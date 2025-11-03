// include/monitor/Engine.hpp
#pragma once
#include <memory>
#include <thread>

#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/os/AbstractMemReader.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <atomic>


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

            // Starts worker thread, (startEngineThread)
            void ignition(ftxui::ScreenInteractive &screen);
            // (stopEngineThread)
            void shutdown();
            
            void tick();

        private:
            std::atomic<bool> isThreadRunning = false;
            std::thread       engineThread;
            void threadFunction(ftxui::ScreenInteractive &screen);
    };

} // namespace monitor