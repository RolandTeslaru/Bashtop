#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/types/Cpu.hpp"


namespace monitor::metrics {

    CpuMonitor::CpuMonitor(std::unique_ptr<monitor::os::AbstractCpuReader> reader){
        cpuReader = std::move(reader);
    }

    CpuMonitor::~CpuMonitor(){

    }

    double CpuMonitor::getCpuTotalUsage(){
        return this->latestSnapshot.total_percentage;
    }

    double CpuMonitor::getCpuCoreUsage(const unsigned int coreIdx){
        bool doesCoreExist = this->latestSnapshot.per_core_percentage.size() > coreIdx + 1;
        if(doesCoreExist == false){
            return 0.0;
        }

        return this->latestSnapshot.per_core_percentage[coreIdx];
    }
 
    void CpuMonitor::computeSnapshot(){
        monitor::types::cpu::RawSample currentSample{};

        if(!this->cpuReader->sample(currentSample))
            return;
        
        if(this->hasSampledOnce == false){
            prevSample = currentSample;
            hasSampledOnce = true;

            latestSnapshot.window_ns = 0;
            latestSnapshot.total_percentage = 0.0;
            latestSnapshot.per_core_percentage.assign(
                currentSample.per_core.size(), 0.0
            );

            return;
        }

        latestSnapshot.window_ns = currentSample.timestamp_ns - prevSample.timestamp_ns;

        latestSnapshot.total_percentage = getTotalPercentage(
            currentSample, 
            prevSample
        );

        // Get the number of cores, it should stay the same but they can change
        const std::size_t prev_num_cores = prevSample.per_core.size();
        const std::size_t cur_num_cores  = currentSample.per_core.size();
        const std::size_t num_cores      = std::min(prev_num_cores, cur_num_cores);

        latestSnapshot.per_core_percentage.assign(num_cores, 0.0);

        for(std::size_t coreIdx = 0; coreIdx < num_cores; ++coreIdx){
            monitor::types::cpu::CoreTicks currCoreTicks = currentSample.per_core[coreIdx];
            monitor::types::cpu::CoreTicks prevCoreTicks = prevSample.per_core[coreIdx];

            latestSnapshot.per_core_percentage[coreIdx] = getCorePercentage(
                currCoreTicks, prevCoreTicks
            );
        }

        this->prevSample = std::move(currentSample);
    }
};