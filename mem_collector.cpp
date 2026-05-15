#include "mem_collector.h"

#include <fstream>
#include <map>
#include <sstream>
#include <string>

void MemCollector::collect(std::vector<MetricSample>& out) {
    std::ifstream fin("/proc/meminfo");
    if (!fin.is_open()) {
        return;
    }

    std::map<std::string, unsigned long long> meminfo;
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string key;
        unsigned long long value;
        std::string unit;
        if (!(iss >> key >> value >> unit)) {
            continue;
        }
        if (!key.empty() && key.back() == ':') {
            key.pop_back();
        }
        meminfo[key] = value; // kB
    }

    unsigned long long mem_total =
        meminfo.count("MemTotal") ? meminfo.at("MemTotal") : 1;
    unsigned long long mem_available =
        meminfo.count("MemAvailable") ? meminfo.at("MemAvailable") : 0;

    unsigned long long used = mem_total - mem_available;
    double usage_percent =
        static_cast<double>(used) / static_cast<double>(mem_total) * 100.0;

    MetricSample s;
    s.name = "mem.usage";
    s.value = usage_percent;
    s.unit = "%";
    out.push_back(s);
}
