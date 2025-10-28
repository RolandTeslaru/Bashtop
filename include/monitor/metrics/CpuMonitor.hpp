#include <memory>
#include "monitor/types/Cpu.hpp"
#include "monitor/os/AbstractCpuReader.hpp"
#include <ostream>

using CpuSnapshot = monitor::types::cpu::Snapshot;

namespace monitor::metrics {
    class CpuMonitor {
        public:
            CpuMonitor(std::unique_ptr<monitor::os::AbstractCpuReader> reader);
            ~CpuMonitor();

            void computeSnapshot();

            double getCpuTotalUsage();
            double getCpuCoreUsage(const unsigned int coreIdx);

            friend std::ostream& operator<<(std::ostream& os, CpuMonitor& mon) {
                os << "snapshot: " 
                   << "windowNs= "         << mon.latestSnapshot.window_ns 
                   << "total_percentage= " << mon.latestSnapshot.total_percentage;

                for(size_t i = 0; i < mon.latestSnapshot.per_core_percentage.size(); i++)
                    os << "core" << i << " = " << mon.latestSnapshot.per_core_percentage[i] << "%";
                
                return os;
            }

        private:
            std::unique_ptr<monitor::os::AbstractCpuReader> cpuReader;
            
            monitor::types::cpu::Snapshot latestSnapshot;
            
            monitor::types::cpu::RawSample prevSample;

            bool hasSampledOnce = false;

            static double toDouble(auto num){
                return static_cast<double>(num);
            }

            static double getTotalPercentage(
                const monitor::types::cpu::RawSample& newSample, 
                const monitor::types::cpu::RawSample& prevSample
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
                const monitor::types::cpu::CoreTicks newCoreTick,
                const monitor::types::cpu::CoreTicks prevCoreTick
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