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
            std::shared_ptr<monitor::metrics::CpuMonitor> cpuMonitor; 
            
            std::shared_ptr<monitor::metrics::MemMonitor> memMonitor;

        public:
            Engine();
            Engine(
                std::unique_ptr<os::AbstractCpuReader> cpuReader,
                std::unique_ptr<os::AbstractMemReader> memReader
            );
            ~Engine() = default;
            
            Engine(const Engine &other);
            Engine& operator=(const Engine &other);

            friend std::ostream& operator<<(std::ostream& os, const Engine& engine);

            std::shared_ptr<monitor::metrics::CpuMonitor> getCpuMonitor();
            [[maybe_unused]] std::shared_ptr<monitor::metrics::MemMonitor> getMemMonitor();

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