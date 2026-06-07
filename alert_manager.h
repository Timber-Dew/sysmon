#pragma once
#include <chrono>
#include "config.h"
#include "metric_collector.h"

struct AlertDecision {
    bool should_alert = false;
    double threshold = 0.0;
};

class AlertManager {
public:
    explicit AlertManager(const SysmonConfig& config);

    AlertDecision check(const MetricSample& sample);

private:
    using SteadyClock = std::chrono::steady_clock;

    SysmonConfig config_;

    int cpu_high_count_ = 0;
    SteadyClock::time_point last_cpu_alarm_time_{};
    bool has_cpu_alarm_time_ = false;

    int mem_high_count_ = 0;
    SteadyClock::time_point last_mem_alarm_time_{};
    bool has_mem_alarm_time_ = false;
};