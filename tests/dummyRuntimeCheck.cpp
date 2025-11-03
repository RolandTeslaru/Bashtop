#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/metrics/SystemInfoProvider.hpp"
#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/os/AbstractMemReader.hpp"
#include "monitor/os/AbstractPlatformInfo.hpp"
#include "monitor/core/Engine.hpp"

using seconds = std::chrono::seconds;

int main (){
    monitor::Engine engine;
    
    auto &cpuMonitor = engine.getCpuMonitor();

    // Get a few cpu samples
    engine.tick();
    std::this_thread::sleep_for(seconds(1));
    engine.tick();
    std::this_thread::sleep_for(seconds(1));
    engine.tick();
    std::this_thread::sleep_for(seconds(1));
    engine.tick();

    std::cout << "CPU_TOTAL_USAGE " << cpuMonitor.getCpuTotalUsage() << "%" << std::endl;
    
    return 0;
}