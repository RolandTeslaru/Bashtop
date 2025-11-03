#pragma once
#include <memory>
#include "monitor/types/Cpu.hpp"
#include "monitor/os/AbstractCpuReader.hpp"
#include <ostream>

using CpuSnapshot       = monitor::types::cpu::Snapshot;
using RawSample         = monitor::types::cpu::RawSample;
using CoreTicks         = monitor::types::cpu::CoreTicks;
using AbstractCpuReader = monitor::os::AbstractCpuReader;
using vector_double     = std::vector<double>;

namespace monitor::metrics {
    class CpuMonitor {
        public:
            explicit CpuMonitor(std::unique_ptr<AbstractCpuReader> reader);
            ~CpuMonitor();

            void computeSnapshot();

            double getCpuTotalUsage();
            double getCpuCoreUsage(const unsigned int coreIdx);
            int getNumCores();

            friend std::ostream& operator<<(std::ostream& os, const CpuMonitor& mon) 
            {
                os << "snapshot: "         << std::endl
                   << "windowNs= "         << mon.latestSnapshot.window_ns << std::endl
                   << "total_percentage= " << mon.latestSnapshot.total_percentage << std::endl;

                for(size_t i = 0; i < mon.latestSnapshot.per_core_percentage.size(); i++)
                    os << "core" << i << " = " << mon.latestSnapshot.per_core_percentage[i] << "%" << std::endl;
                
                return os;
            }

            const vector_double getCpuUsageHistory();
            [[maybe_unused]] const vector_double getCoreUsageHistory(const unsigned int coreIdx);

        private:
            std::unique_ptr<AbstractCpuReader> cpuReader;
            
            RawSample   prevSample;
            CpuSnapshot latestSnapshot;

            bool hasSampledOnce = false;

            size_t num_cores; // number of cores
            
            // History
            vector_double cpu_usage_history;
            std::vector<vector_double> core_usage_history;

            // Helpers
            static double toDouble(auto num){
                return static_cast<double>(num);
            }
            
            static double getTotalPercentage(
                const RawSample& newSample, 
                const RawSample& prevSample
            ){
                const double idle_delta  = toDouble(newSample.total.idle  - prevSample.total.idle);
                const double total_delta = toDouble(newSample.total.total - prevSample.total.total);

                double total_percentage = 0.0;
                if(total_delta != 0){
                    total_percentage = (1.0 - (idle_delta / total_delta)) * 100.0;

                    if(total_percentage < 0.0)
                        total_percentage = 0.0;
                    else if(total_percentage > 100.0)
                        total_percentage = 100.0;
                }

                return total_percentage;
            }

            static double getCorePercentage(
                const CoreTicks newCoreTick,
                const CoreTicks prevCoreTick
            ){
                const double idle_delta  = toDouble(newCoreTick.idle  - prevCoreTick.idle);
                const double total_delta = toDouble(newCoreTick.total - prevCoreTick.total);

                double core_percentage = 0.0;
                if(total_delta != 0){
                    core_percentage = (1.0 - (idle_delta / total_delta)) * 100.0;
                    if(core_percentage < 0.0)
                        core_percentage = 0.0;
                    else if(core_percentage > 100.0)
                        core_percentage = 100.0;
                }
                return core_percentage;
            }
    };
}