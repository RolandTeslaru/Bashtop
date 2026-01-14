#pragma once
#include <memory>
#include "monitor/types/Cpu.hpp"
#include "monitor/types/RollingHistory.hpp"
#include "monitor/os/AbstractCpuReader.hpp"
#include <ostream>

using CpuSnapshot       = monitor::types::cpu::Snapshot;
using CpuRawSample      = monitor::types::cpu::RawSample;
using CoreTicks         = monitor::types::cpu::CoreTicks;
using AbstractCpuReader = monitor::os::AbstractCpuReader;
using vector_double     = std::vector<double>;

namespace monitor::metrics {
    class CpuMonitor {
        public:
            // ====================================================================
            // Constructor / Destructor
            // ====================================================================
            explicit CpuMonitor(std::unique_ptr<AbstractCpuReader> reader);
            ~CpuMonitor();

            // ====================================================================
            // Copy Constructor / Operator Overloads
            // ====================================================================
            CpuMonitor(const CpuMonitor& other);
            
            friend std::ostream& operator<<(std::ostream& os, const CpuMonitor& mon);
            
            CpuMonitor& operator=(CpuMonitor other);

            friend void swap(CpuMonitor& Mon1, CpuMonitor& Mon2) noexcept;

            // ====================================================================
            // Public Interface
            // ====================================================================
            void   computeSnapshot();
            int    getNumCores();
            double getCpuTotalUsage();
            double getCpuCoreUsage(const unsigned int coreIdx);
            const vector_double getCpuUsageHistory();
            [[maybe_unused]] const vector_double getCoreUsageHistory(const unsigned int coreIdx);

        private:
            // ====================================================================
            // Member Variables
            // ====================================================================
            static const std::size_t HISTORY_LIMIT = 240;

            std::unique_ptr<AbstractCpuReader> cpuReader;

            CpuRawSample   prevSample;
            CpuSnapshot    latestSnapshot;
            bool           snapshot_valid = false; // whether latestSnapshot reflects a successful sample

            bool   hasSampledOnce = false;
            size_t num_cores;  // number of cores

            // History
            monitor::types::RollingHistory<double> cpu_usage_history = monitor::types::RollingHistory<double>(HISTORY_LIMIT);
            std::vector<monitor::types::RollingHistory<double>> core_usage_history;
    };
}