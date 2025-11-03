// include/monitor/Engine.hpp
#pragma once
#include <memory>
#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/os/AbstractMemReader.hpp"


namespace monitor {
    class Engine {
        private:
            monitor::metrics::CpuMonitor cpuMonitor;
            
            monitor::metrics::MemMonitor memMonitor;

        public:
            Engine();
            ~Engine() = default;

            Engine(
                std::unique_ptr<os::AbstractCpuReader> cpuReader,
                std::unique_ptr<os::AbstractMemReader> memReader
            );

            monitor::metrics::CpuMonitor& getCpuMonitor();
            [[maybe_unused]] monitor::metrics::MemMonitor& getMemMonitor();

            void tick();
        private:
    };

} // namespace monitor