#pragma once

#include <memory>
#include <vector>
#include <chrono>
#include "logger.h"
#include "metric_collector.h"

// 核心服务：调度所有采集器，并统一打印指标
class SysmonService {
public:
    explicit SysmonService(double intervalSec);

    // 注册一个采集器
    void addCollector(std::unique_ptr<IMetricCollector> c);

    // 主循环：定时采集并打印
    void run();

    Logger& logger() { return logger_; }

private:
    using SteadyClock = std::chrono::steady_clock;

private:
    double intervalSec_;
    Logger logger_;
    std::vector<std::unique_ptr<IMetricCollector>> collectors_;

    // 阶段1：稳定节拍 + 本轮循环耗时
    SteadyClock::time_point next_tick_{};
    double loop_cost_ms_{ 0.0 };
};
