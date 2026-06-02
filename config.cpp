#include "config.h"

#include <fstream>
#include <sstream>
#include <string>

SysmonConfig loadConfig(const std::string& path) {
    SysmonConfig config;

    std::ifstream fin(path);
    if (!fin.is_open()) {
        return config; // 返回默认配置
    }

    std::string line;
    while (std::getline(fin, line)) {
        // 跳过空行和注释
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string key, value;

        if(!std::getline(iss, key, '=') ) {
            continue; // 格式错误，跳过
        }
         if(!std::getline(iss, value)) {
            continue; // 格式错误，跳过
        }

  
        if (key == "interval_sec") {
            config.interval_sec = std::stod(value);
        } else if (key == "cpu_threshold") {
            config.cpu_threshold = std::stod(value);
        } else if (key == "mem_threshold") {
            config.mem_threshold = std::stod(value);
        } else if (key == "alert_consecutive_count") {
            config.alert_consecutive_count = std::stod(value);
        } else if (key == "alert_cooldown_sec") {
            config.alert_cooldown_sec = std::stod(value);
        }
        
    }

    return config;
}