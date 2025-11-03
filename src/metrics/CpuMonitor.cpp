#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/types/Cpu.hpp"

using size_t            = std::size_t;

using CoreTicks         = monitor::types::cpu::CoreTicks;
using RawSample         = monitor::types::cpu::RawSample;
using AbstractCpuReader = monitor::os::AbstractCpuReader;

using vector_double     = std::vector<double>;

namespace monitor::metrics {

    CpuMonitor::CpuMonitor(std::unique_ptr<AbstractCpuReader> reader)
    : cpuReader(std::move(reader))
    {
        // Run an intial snapshot to get the cores;
        RawSample sample;
        cpuReader->sample(sample);

        num_cores = sample.per_core.size(); // get the number of cores from an initial dummy sample
    }

    CpuMonitor::~CpuMonitor(){

    }

    int CpuMonitor::getNumCores(){
        return this->num_cores;
    }

    double CpuMonitor::getCpuTotalUsage(){
        return this->latestSnapshot.total_percentage;
    }

    double CpuMonitor::getCpuCoreUsage(const unsigned int coreIdx){
        bool doesCoreExist = this->latestSnapshot.per_core_percentage.size() > coreIdx;
        if(doesCoreExist == false){
            return 0.0;
        }

        return this->latestSnapshot.per_core_percentage[coreIdx];
    }

    const vector_double CpuMonitor::getCpuUsageHistory(){
        return this->cpu_usage_history;
    }
    const vector_double CpuMonitor::getCoreUsageHistory(
        const unsigned int coreIdx
    ){
        bool doesCoreExist = this->core_usage_history.size() > coreIdx;
        if(doesCoreExist == false){
            static const vector_double empty;
            return empty;
        }

        return this->core_usage_history[coreIdx];
    }
 
    void CpuMonitor::computeSnapshot(){
        RawSample currentSample{};

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

            core_usage_history.assign(currentSample.per_core.size(), vector_double());

            return;
        }

        latestSnapshot.window_ns = currentSample.timestamp_ns - prevSample.timestamp_ns;




        // Push Total Cpu Usage into histroy

        double processor_usage = getTotalPercentage(
            currentSample, 
            prevSample
        );

        latestSnapshot.total_percentage = processor_usage;  

        this->cpu_usage_history.push_back(processor_usage);



        // Push per-core Cpu Usage into history

        // Get the number of cores, it should stay the same but they can change
        const size_t prev_num_cores = prevSample.per_core.size();
        const size_t cur_num_cores  = currentSample.per_core.size();

        this->num_cores = std::min(prev_num_cores, cur_num_cores);

        latestSnapshot.per_core_percentage.assign(this->num_cores, 0.0);

        for(size_t coreIdx = 0; coreIdx < this->num_cores; ++coreIdx){
            CoreTicks currCoreTicks = currentSample.per_core[coreIdx];
            CoreTicks prevCoreTicks = prevSample.per_core[coreIdx];

            double corePercentage = getCorePercentage(
                currCoreTicks, prevCoreTicks
            );

            latestSnapshot.per_core_percentage[coreIdx] = corePercentage;

            this->core_usage_history[coreIdx].push_back(corePercentage);
        }

        this->prevSample = std::move(currentSample);
    }



};