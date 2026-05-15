#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "metric_sample.h"   // ? 复用工程内已有 MetricSample

class Logger {
public:
    explicit Logger(const std::string& jsonl_path = "metrics.jsonl");

    void info(const std::string& msg);
    void warn(const std::string& msg);

    // ===== 阶段2新增 =====
    std::string nowIso8601Local() const;

    void metricsJsonl(const std::string& ts,
        const std::vector<MetricSample>& samples);

    void alarmJsonl(const std::string& ts,
        const std::string& metric,
        double value,
        double threshold,
        const std::string& snapshot_dir);

private:
    std::ofstream jsonl_;

    static std::string jsonEscape(const std::string& s);
    void appendLine(const std::string& line);
};
