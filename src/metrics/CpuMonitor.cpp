#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>

#include <iomanip>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/types/Cpu.hpp"
#include "monitor/ansi.hpp"
#include "monitor/core/TemplateUtils.hpp"
#include "monitor/exceptions/SampleExceptions.hpp"
#include <utility>

#include <iostream>

using size_t = std::size_t;

using CoreTicks = monitor::types::cpu::CoreTicks;
using CpuRawSample = monitor::types::cpu::RawSample;
using AbstractCpuReader = monitor::os::AbstractCpuReader;

using vector_double = std::vector<double>;

namespace monitor::metrics {

    using CpuSampleException = monitor::exceptions::CpuSampleException;
    // ============================================================================
    // Internal Static Helpers
    // ============================================================================
    namespace {
        double computeTotalPercentage(
            const CpuRawSample &newSample,
            const CpuRawSample &prevSample
        ) {
            const double idle_delta = static_cast<double>(newSample.total.idle - prevSample.total.idle);
            const double total_delta = static_cast<double>(newSample.total.total - prevSample.total.total);

            double total_percentage = 0.0;
            if (total_delta != 0) {
                total_percentage = (1.0 - (idle_delta / total_delta)) * 100.0;
                total_percentage = monitor::core::clamp_value(total_percentage, 0.0, 100.0);
            }

            return total_percentage;
        }

        double computeCorePercentage(
            const CoreTicks newCoreTick,
            const CoreTicks prevCoreTick) {
            const double idle_delta = static_cast<double>(newCoreTick.idle - prevCoreTick.idle);
            const double total_delta = static_cast<double>(newCoreTick.total - prevCoreTick.total);

            double core_percentage = 0.0;
            if (total_delta != 0) {
                core_percentage = (1.0 - (idle_delta / total_delta)) * 100.0;
                core_percentage = monitor::core::clamp_value(core_percentage, 0.0, 100.0);
            }
            return core_percentage;
        }
    }



    // ============================================================================
    // Constructor / Destructor
    // ============================================================================
    CpuMonitor::CpuMonitor(std::unique_ptr<AbstractCpuReader> reader)
        : cpuReader(std::move(reader)) 
    {
        // Run an intial snapshot to get the cores;
        CpuRawSample sample{};

        try {
            cpuReader->sample(sample);
        } catch (const CpuSampleException& e) {
            std::cerr << "CPU sampling error during CpuMonitor construction: " << e.what() << std::endl;
            throw;
        }

        num_cores = sample.per_core.size(); // get number of cores from an initial dummy sample
    }



    CpuMonitor::~CpuMonitor() {}


    CpuMonitor::CpuMonitor(const CpuMonitor& other)
        : cpuReader(other.cpuReader->clone()),
          prevSample(other.prevSample),
          latestSnapshot(other.latestSnapshot),
          snapshot_valid(other.snapshot_valid),
          hasSampledOnce(other.hasSampledOnce),
          num_cores(other.num_cores),
          cpu_usage_history(other.cpu_usage_history),
          core_usage_history(other.core_usage_history) {}


    // ============================================================================
    // Operator Overloads
    // ============================================================================
    std::ostream &operator<<(std::ostream &os, const CpuMonitor &mon) {
        os << monitor::ansi::BOLD << monitor::ansi::CYAN << "CpuMonitor: " << monitor::ansi::RESET << std::endl;
        os << "  Number of Cores: " << mon.num_cores << std::endl;
        os << "  Latest Snapshot: " << std::endl;
        os << "    Window (nanoseconds): " << mon.latestSnapshot.window_ns << std::endl;
        os << "    Total Usage (%): " << mon.latestSnapshot.total_percentage << std::endl;
        os << "  Reader: " << *mon.cpuReader; // deref unique ptr
        os << "  Per Core Usage: ";
        if (mon.latestSnapshot.per_core_percentage.empty()) {
            os << "No usage because only the dummy sample in the constructor was taken" << std::endl;
            return os;
        }
        os << std::endl;

        const int labelWidth = 10; // width for the core label column

        for (size_t i = 0; i < mon.latestSnapshot.per_core_percentage.size(); ++i) {
            os << "    " << std::left << std::setw(labelWidth) << ("core " + std::to_string(i) + ":") << " " << mon.latestSnapshot.per_core_percentage[i] << "%" << std::endl;
        }
        os << std::endl;
        return os;
    }

    CpuMonitor& CpuMonitor::operator=(CpuMonitor other){
        swap(*this, other);
        return *this;
    }

    void swap(CpuMonitor& Mon1, CpuMonitor& Mon2) noexcept {
        std::swap(Mon1.cpuReader, Mon2.cpuReader);
        std::swap(Mon1.prevSample, Mon2.prevSample);
        std::swap(Mon1.latestSnapshot, Mon2.latestSnapshot);
        std::swap(Mon1.snapshot_valid, Mon2.snapshot_valid);
        std::swap(Mon1.hasSampledOnce, Mon2.hasSampledOnce);
        std::swap(Mon1.num_cores, Mon2.num_cores);
        std::swap(Mon1.cpu_usage_history, Mon2.cpu_usage_history);
        std::swap(Mon1.core_usage_history, Mon2.core_usage_history);
    }


    // ============================================================================
    // Public Getters
    // ============================================================================
    int CpuMonitor::getNumCores() {
        return this->num_cores;
    }



    double CpuMonitor::getCpuTotalUsage() {
        return this->latestSnapshot.total_percentage;
    }



    double CpuMonitor::getCpuCoreUsage(const unsigned int coreIdx) {
        bool doesCoreExist = this->latestSnapshot.per_core_percentage.size() > coreIdx;
        if (doesCoreExist == false) {
            return 0.0;
        }

        return this->latestSnapshot.per_core_percentage[coreIdx];
    }



    const vector_double CpuMonitor::getCpuUsageHistory() {
        return this->cpu_usage_history.values();
    }



    [[maybe_unused]] const vector_double CpuMonitor::getCoreUsageHistory(
        const unsigned int coreIdx) {
        bool doesCoreExist = this->core_usage_history.size() > coreIdx;
        if (doesCoreExist == false) {
            static const vector_double empty;
            return empty;
        }

        return this->core_usage_history[coreIdx].values();
    }



    
    // ============================================================================
    // Core Logic
    // ============================================================================
    void CpuMonitor::computeSnapshot()
    {
        CpuRawSample currentSample{};

        try {
            this->cpuReader->sample(currentSample);
        }
        catch (const CpuSampleException& e) {
            // mark snapshot invalid and keep previous values
            this->snapshot_valid = false;
            this->latestSnapshot.window_ns = 0;
            std::cerr << "[CpuMonitor] Sampling failed: " << e.what() << std::endl;
            return;
        }

        if (this->hasSampledOnce == false)
        {
            prevSample = currentSample;
            hasSampledOnce = true;

            latestSnapshot.window_ns = 0;
            latestSnapshot.total_percentage = 0.0;
            latestSnapshot.per_core_percentage.assign(
                currentSample.per_core.size(), 0.0
            );

            core_usage_history.assign(
                currentSample.per_core.size(),
                monitor::types::RollingHistory<double>(CpuMonitor::HISTORY_LIMIT)
            );

            // initial dummy sample taken; we don't have a computed snapshot yet
            this->snapshot_valid = false;

            return;
        }

        latestSnapshot.window_ns = currentSample.timestamp_ns - prevSample.timestamp_ns;

        // Push Total Cpu Usage into histroy

        double processor_usage = computeTotalPercentage(currentSample, prevSample);

        latestSnapshot.total_percentage = processor_usage;

        this->cpu_usage_history.push(processor_usage);


        // Get the number of cores, it should stay the same but they can change
        const size_t prev_num_cores = prevSample.per_core.size();
        const size_t cur_num_cores = currentSample.per_core.size();

        this->num_cores = std::min(prev_num_cores, cur_num_cores);

        latestSnapshot.per_core_percentage.assign(this->num_cores, 0.0);

        // Compute per core usage
        for (size_t coreIdx = 0; coreIdx < this->num_cores; ++coreIdx)
        {
            CoreTicks currCoreTicks = currentSample.per_core[coreIdx];
            CoreTicks prevCoreTicks = prevSample.per_core[coreIdx];

            double corePercentage = computeCorePercentage(
                currCoreTicks, prevCoreTicks);

            latestSnapshot.per_core_percentage[coreIdx] = corePercentage;

            this->core_usage_history[coreIdx].push(corePercentage);
        }

        this->prevSample = std::move(currentSample);
        this->snapshot_valid = true;
    }

};