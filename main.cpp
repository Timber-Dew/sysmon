#include <memory>

#include "sysmon_service.h"
#include "cpu_collector.h"
#include "mem_collector.h"

int main() {
    // 每 2 秒采样一次
    SysmonService service(2.0);

    // 注册 CPU / 内存采集器
    service.addCollector(std::make_unique<CpuCollector>());
    service.addCollector(std::make_unique<MemCollector>());

    // 进入主循环
    service.run();
    return 0;
}
