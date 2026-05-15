#include "cpu_collector.h"

#include <fstream>
#include <sstream>
#include <string>

CpuCollector::CpuCollector()
    : hasPrev_(false) {
}

bool CpuCollector::readTimes(CpuTimes& out) const {
    std::ifstream fin("/proc/stat");
    if (!fin.is_open()) {
        return false;
    }

    std::string line;
    if (!std::getline(fin, line)) {
        return false;
    }

    std::istringstream iss(line);
    std::string cpu_label;
    iss >> cpu_label; // "cpu"

    CpuTimes t;
    iss >> t.user >> t.nice >> t.system >> t.idle >> t.iowait
        >> t.irq >> t.softirq >> t.steal >> t.guest >> t.guest_nice;

    t.idle_all = t.idle + t.iowait;
    t.total = t.user + t.nice + t.system + t.idle + t.iowait +
        t.irq + t.softirq + t.steal + t.guest + t.guest_nice;

    out = t;
    return true;
}

double CpuCollector::calcUsage(const CpuTimes& prev,
    const CpuTimes& curr) const {
    unsigned long long totald = curr.total - prev.total;
    unsigned long long idled = curr.idle_all - prev.idle_all;

    if (totald == 0) {
        return 0.0;
    }

    double usage = (static_cast<double>(totald - idled) /
        static_cast<double>(totald)) * 100.0;
    return usage;
}

void CpuCollector::collect(std::vector<MetricSample>& out) {
    CpuTimes curr;
    if (!readTimes(curr)) {
        return;
    }

    if (!hasPrev_) {
        prev_ = curr;
        hasPrev_ = true;
        // 第一次采样先记录基准，不输出
        return;
    }

    double usage = calcUsage(prev_, curr);
    prev_ = curr;

    MetricSample s;
    s.name = "cpu.usage";
    s.value = usage;
    s.unit = "%";
    out.push_back(s);
}
