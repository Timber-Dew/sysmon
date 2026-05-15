#pragma once
#include "metric_collector.h"

// /proc/stat 中的一次 CPU 时间快照
struct CpuTimes {
    unsigned long long user = 0;
    unsigned long long nice = 0;
    unsigned long long system = 0;
    unsigned long long idle = 0;
    unsigned long long iowait = 0;
    unsigned long long irq = 0;
    unsigned long long softirq = 0;
    unsigned long long steal = 0;
    unsigned long long guest = 0;
    unsigned long long guest_nice = 0;

    unsigned long long idle_all = 0; // idle + iowait
    unsigned long long total = 0; // 所有字段之和
};

class CpuCollector : public IMetricCollector {
public:
    CpuCollector();

    void collect(std::vector<MetricSample>& out) override;
    const char* name() const override { return "cpu"; }

private:
    bool   readTimes(CpuTimes& out) const;
    double calcUsage(const CpuTimes& prev, const CpuTimes& curr) const;

private:
    bool     hasPrev_;
    CpuTimes prev_;
};
