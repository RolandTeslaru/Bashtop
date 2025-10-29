#include <iostream>
#include <chrono>
#include <thread>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/SystemInfoProvider.hpp"
#include "monitor/os/Factory.hpp"

using namespace ftxui;

int main()
{


    auto platformInfo = monitor::metrics::SystemInfoProvider(
        monitor::os::make_platform_info()
    );

    std::cout << platformInfo.getPlatformInfo() << std::endl;

    while(true){
        cpuMonitor.computeSnapshot();
        std::cout << "CPU total usage: " << cpuMonitor.getCpuTotalUsage() << "%" << std::endl << std::endl;
        std::cout << cpuMonitor << std::endl << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
