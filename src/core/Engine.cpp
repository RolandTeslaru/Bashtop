#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"


namespace monitor {
    Engine::Engine()
    : cpuMonitor(monitor::os::make_cpu_reader()),
        memMonitor(monitor::os::make_mem_reader())
    {}

    Engine::Engine(
        std::unique_ptr<os::AbstractCpuReader> cpuReader,
        std::unique_ptr<os::AbstractMemReader> memReader
    )
    :   cpuMonitor(std::move(cpuReader)),
        memMonitor(std::move(memReader))
    {}

    void Engine::tick(){
        this->cpuMonitor.computeSnapshot();
    }

    monitor::metrics::CpuMonitor& Engine::getCpuMonitor(){
        return this->cpuMonitor;
    }

    [[maybe_unused]] monitor::metrics::MemMonitor& Engine::getMemMonitor(){
        return this->memMonitor;
    }
}