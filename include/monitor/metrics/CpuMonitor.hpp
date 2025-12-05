#pragma once
#include <memory>
#include "monitor/types/Cpu.hpp"
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
            // Operator Overloads
            // ====================================================================
            friend std::ostream& operator<<(std::ostream& os, const CpuMonitor& mon);

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
            std::unique_ptr<AbstractCpuReader> cpuReader;

            CpuRawSample   prevSample;
            CpuSnapshot    latestSnapshot;

            bool   hasSampledOnce = false;
            size_t num_cores;  // number of cores

            // History
            vector_double cpu_usage_history;
            std::vector<vector_double> core_usage_history;
    };
}