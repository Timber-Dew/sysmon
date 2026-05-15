#pragma once
#include <string>

struct MetricSample {
    std::string name;  // 指标名，比如 "cpu.usage"
    double      value; // 数值
    std::string unit;  // 单位，比如 "%"
};
