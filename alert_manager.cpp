#include "alert_manager.h"

AlertManager::AlertManager(const SysmonConfig& config)
    : config_(config) {
}

AlertDecision AlertManager::check(const MetricSample& sample) {
    
    AlertDecision decision;

    if (sample.name == "cpu.usage") {
        if (sample.value > config_.cpu_threshold) {
            cpu_high_count_++;
        } else {
            cpu_high_count_ = 0;
        }

        auto now = SteadyClock::now();
        bool cooldown_passed = !has_cpu_alarm_time_ || std::chrono::duration_cast<std::chrono::seconds>(now - last_cpu_alarm_time_).count() >= config_.alert_cooldown_sec;

        if (cpu_high_count_ >= config_.alert_consecutive_count && cooldown_passed) {
            decision.should_alert = true;
            decision.threshold = config_.cpu_threshold;
            last_cpu_alarm_time_ = now;
            has_cpu_alarm_time_ = true;
        }
    }

    return decision;
}