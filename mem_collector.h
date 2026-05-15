#pragma once
#include "metric_collector.h"

class MemCollector : public IMetricCollector {
public:
    void collect(std::vector<MetricSample>& out) override;
    const char* name() const override { return "mem"; }
};
