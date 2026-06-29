#include "sysmon_service.h"
#include "config.h"
#include "alert_manager.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>   // std::system
#include <iomanip>
#include <sstream>
#include <thread>
#include <iostream>

// 阈值（百分比）
SysmonService::SysmonService(const SysmonConfig& config)
    : config_(config),
    alert_manager_(config){
}

void SysmonService::addCollector(std::unique_ptr<IMetricCollector> c) {
    collectors_.push_back(std::move(c));
}

void SysmonService::run() {
    logger_.info("sysmon service started");

    using namespace std::chrono;

    // 用 steady_clock 保证不受系统时间回拨影响
    const auto interval = duration_cast<SteadyClock::duration>(duration<double>(config_.interval_sec));
    next_tick_ = SteadyClock::now() + interval;


    while (true) {
        // 阶段2：统一时间戳（本轮 metrics / alarm / snapshot 复用）
        const std::string ts = logger_.nowIso8601Local();
        const auto loop_begin = SteadyClock::now();

        std::vector<MetricSample> samples;

        // 让所有采集器填充本轮 samples
        for (auto& c : collectors_) {
            auto start = SteadyClock::now();
            bool ok = true;
            long long cost_us = 0.0;
            try
            {          
                c->collect(samples);
                auto end = SteadyClock::now();
                cost_us = duration<double, std::micro>(end - start).count();
            }
            catch(const std::ios_base::failure& e)
            {
                ok = false;
                std::cerr << "[Collector Error] " << c->name() << " failed: " << e.what() << std::endl;
            }
            catch(const std::invalid_argument& e)
            {
                ok = false;
                std::cerr << "[Collector Error] " << c->name() << " invalid data: " << e.what() << std::endl;
            }
            catch(const std::out_of_range& e)
            {
                ok = false;
                std::cerr << "[Collector Error] " << c->name() << " out of range: " << e.what() << std::endl;
            }
             catch(const std::exception& e)
            {
                ok = false;
                std::cerr << "[Collector Error] " << c->name() << " unknown error" << std::endl;
            }
            catch(...)
            {
                ok = false; 
                std::cerr << "[Collector Error] " << c->name() << " unknown non-exception error" << std::endl;
            }

            samples.push_back(MetricSample{std::string("collector.") + c->name() + ".ok" , ok ? 1.0 : 0.0 , " "});

            samples.push_back(MetricSample{std::string("collector.") + c->name() + ".cost_us" , static_cast<double>(cost_us) , "us"});
            
        }

        // 阶段2：同一轮只生成一次快照，避免 cpu+mem 同时超阈值重复落盘
        bool snapshot_taken = false;
        std::string snapshot_dir;

        // 生成一个“文件系统友好”的目录名：把 ':' 替换成 '-'
        auto makeSafeDir = [&](const std::string& base) -> std::string {
            std::string s = base;
            std::replace(s.begin(), s.end(), ':', '-');
            return s;
            };

        // === 阈值告警逻辑（阶段2：告警 JSONL + 现场快照） ===
        for (const auto& s : samples) {
            AlertDecision decision_ = alert_manager_.check(s);
            if (decision_.should_alert){
                std::ostringstream w;
                w << "high: "
                    << std::fixed << std::setprecision(1)
                    << s.value << s.unit
                    << " (threshold=" << decision_.threshold << "%)";
                logger_.warn(w.str());
            

                if (!snapshot_taken) {
                    std::string reason = s.name == "cpu.usage" ? "cpu_high" : "mem_high";
                    snapshot_dir = makeSafeDir("snapshots/" + ts + "_" + reason);
                    std::string cmd = "scripts/snapshot.sh " + snapshot_dir;
                    int rc=std::system(cmd.c_str());
                    if(rc!=0){
                        logger_.warn("snapshot command failed " + cmd);
                    }
                    snapshot_taken = true;
                }

                // 告警事件写入 JSONL（便于检索/统计/关联快照）
                logger_.alarmJsonl(ts, s.name, s.value, decision_.threshold, snapshot_dir);/* code */
            }
        }

        // 阶段1：记录本轮循环耗时（ms）
        const auto loop_end = SteadyClock::now();
        loop_cost_ms_ = duration<double, std::milli>(loop_end - loop_begin).count();
        samples.push_back(MetricSample{ "service.loop_cost_ms", loop_cost_ms_, "ms" });

        // 阶段2：每轮把指标写入 metrics.jsonl（一行一个 JSON）
        logger_.metricsJsonl(ts, samples);

        // 终端可读日志（保留你原来的输出）
        if (!samples.empty()) {
            std::ostringstream oss;
            bool first = true;
            for (const auto& sm : samples) {
                if (!first) oss << "  ";
                first = false;
                oss << sm.name << "="
                    << std::fixed << std::setprecision(1)
                    << sm.value << sm.unit;
            }
            logger_.info(oss.str());
        }

        // 阶段1：稳定节拍 sleep_until(next_tick_)
        const auto now = SteadyClock::now();
        if (now < next_tick_) {
            std::this_thread::sleep_until(next_tick_);
            next_tick_ += interval;
        }
        else {
            const auto behind = now - next_tick_;
            const auto skip = (behind / interval) + 1;
            next_tick_ += skip * interval;
        }
    }
}
