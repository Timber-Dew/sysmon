#pragma once

#include <string>

 struct SysmonConfig {
    double interval_sec=2.0;
    double cpu_threshold=80.0;
    double mem_threshold=80.0;
};

SysmonConfig loadConfig(const std::string& path);