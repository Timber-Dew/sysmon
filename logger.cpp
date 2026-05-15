#include "logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

Logger::Logger(const std::string& jsonl_path) {
    jsonl_.open(jsonl_path, std::ios::app);
}

void Logger::info(const std::string& msg) {
    std::cout << "[" << nowIso8601Local() << "] [INFO] " << msg << std::endl;
}

void Logger::warn(const std::string& msg) {
    std::cout << "[" << nowIso8601Local() << "] [WARN] " << msg << std::endl;
}

std::string Logger::nowIso8601Local() const {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);

    std::tm tm{};
    localtime_r(&t, &tm);

    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
        << "." << std::setw(3) << std::setfill('0') << ms.count()
        << "+09:00";  // 你在日本时区；阶段6再做配置化
    return oss.str();
}

void Logger::metricsJsonl(const std::string& ts,
    const std::vector<MetricSample>& samples) {
    std::ostringstream oss;
    oss << "{\"ts\":\"" << ts << "\",\"level\":\"INFO\",\"event\":\"metrics\",\"metrics\":{";

    bool first = true;
    for (const auto& s : samples) {
        if (!first) oss << ",";
        first = false;
        oss << "\"" << jsonEscape(s.name) << "\":" << s.value;
    }

    oss << "},\"unit\":{";
    first = true;
    for (const auto& s : samples) {
        if (!first) oss << ",";
        first = false;
        oss << "\"" << jsonEscape(s.name) << "\":\"" << jsonEscape(s.unit) << "\"";
    }
    oss << "}}";

    appendLine(oss.str());
}

void Logger::alarmJsonl(const std::string& ts,
    const std::string& metric,
    double value,
    double threshold,
    const std::string& snapshot_dir) {
    std::ostringstream oss;
    oss << "{\"ts\":\"" << ts
        << "\",\"level\":\"WARN\",\"event\":\"alarm\""
        << ",\"metric\":\"" << jsonEscape(metric)
        << "\",\"value\":" << value
        << ",\"threshold\":" << threshold
        << ",\"snapshot_dir\":\"" << jsonEscape(snapshot_dir)
        << "\"}";

    appendLine(oss.str());
}

std::string Logger::jsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        switch (c) {
        case '\"': out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\n': out += "\\n";  break;
        case '\r': out += "\\r";  break;
        case '\t': out += "\\t";  break;
        default:   out += c;      break;
        }
    }
    return out;
}

void Logger::appendLine(const std::string& line) {
    if (jsonl_.is_open()) {
        jsonl_ << line << "\n";
        jsonl_.flush();
    }
}
