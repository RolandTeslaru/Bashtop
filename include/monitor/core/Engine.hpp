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
        public:
            // ====================================================================
            // Constructor / Destructor
            // ====================================================================
            Engine();
            Engine(
                std::unique_ptr<os::AbstractCpuReader> cpuReader,
                std::unique_ptr<os::AbstractMemReader> memReader
            );
            ~Engine();

            // ====================================================================
            // Operator Overloads
            // ====================================================================
            Engine(const Engine &other);
            Engine& operator=(Engine other);
            friend std::ostream& operator<<(std::ostream& os, const Engine& engine);
            
            friend void swap(Engine& Eng1, Engine& Eng2) noexcept;

            // ====================================================================
            // Public Interface
            // ====================================================================
            std::shared_ptr<monitor::metrics::CpuMonitor> getCpuMonitor();
            [[maybe_unused]] std::shared_ptr<monitor::metrics::MemMonitor> getMemMonitor();

            // Starts worker thread (ignition sounds cooler)
            void ignition(ftxui::ScreenInteractive &screen);
            // Stops worker thread
            void shutdown();

            void tick();

        private:
            // ====================================================================
            // Private Helpers
            // ====================================================================
            void threadFunction(ftxui::ScreenInteractive &screen);

            // ====================================================================
            // Member Variables
            // ====================================================================
            std::shared_ptr<monitor::metrics::CpuMonitor> cpuMonitor;
            std::shared_ptr<monitor::metrics::MemMonitor> memMonitor;

            std::atomic<bool> isThreadRunning = false;
            std::thread engineThread;
    };

} 