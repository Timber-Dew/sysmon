#include <memory>

#include "sysmon_service.h"
#include "cpu_collector.h"
#include "mem_collector.h"
#include "config.h"

int main() {
    // 每 2 秒采样一次
    SysmonConfig config = loadConfig("sysmon.conf");
    SysmonService service(config);

    // 注册 CPU / 内存采集器
    service.addCollector(std::make_unique<CpuCollector>());
    service.addCollector(std::make_unique<MemCollector>());

    // 进入主循环
    service.run();
    return 0;
}
