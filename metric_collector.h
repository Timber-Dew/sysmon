#pragma once
#include <vector>
#include "metric_sample.h"

// 所有“采集器”的通用接口
class IMetricCollector {
public:
    virtual ~IMetricCollector() = default;

    // 采集一次，把结果 push 到 out
    virtual void collect(std::vector<MetricSample>& out) = 0;

    // 采集器名字（可选，用于调试/日志）
    virtual const char* name() const = 0;
};
