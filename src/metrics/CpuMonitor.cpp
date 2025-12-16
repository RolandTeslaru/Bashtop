#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>

#include <iomanip>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/types/Cpu.hpp"
#include "monitor/ansi.hpp"
#include "monitor/exceptions/SampleExceptions.hpp"

using size_t = std::size_t;

using CoreTicks = monitor::types::cpu::CoreTicks;
using CpuRawSample = monitor::types::cpu::RawSample;
using AbstractCpuReader = monitor::os::AbstractCpuReader;

using vector_double = std::vector<double>;

namespace monitor::metrics {

    // ============================================================================
    // Internal Static Helpers
    // ============================================================================
    namespace {
        template <typename T>
        double toDouble(T num) {
            return static_cast<double>(num);
        }

        double computeTotalPercentage(
            const CpuRawSample &newSample,
            const CpuRawSample &prevSample
        ) {
            const double idle_delta = toDouble(newSample.total.idle - prevSample.total.idle);
            const double total_delta = toDouble(newSample.total.total - prevSample.total.total);

            double total_percentage = 0.0;
            if (total_delta != 0) {
                total_percentage = (1.0 - (idle_delta / total_delta)) * 100.0;

                if (total_percentage < 0.0)
                    total_percentage = 0.0;
                else if (total_percentage > 100.0)
                    total_percentage = 100.0;
            }

            return total_percentage;
        }

        double computeCorePercentage(
            const CoreTicks newCoreTick,
            const CoreTicks prevCoreTick) {
            const double idle_delta = toDouble(newCoreTick.idle - prevCoreTick.idle);
            const double total_delta = toDouble(newCoreTick.total - prevCoreTick.total);

            double core_percentage = 0.0;
            if (total_delta != 0) {
                core_percentage = (1.0 - (idle_delta / total_delta)) * 100.0;
                if (core_percentage < 0.0)
                    core_percentage = 0.0;
                else if (core_percentage > 100.0)
                    core_percentage = 100.0;
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
        CpuRawSample sample;
        cpuReader->sample(sample);

        num_cores = sample.per_core.size(); // get number of cores from an initial dummy sample
    }

    CpuMonitor::~CpuMonitor() {}

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
        return this->cpu_usage_history;
    }

    [[maybe_unused]] const vector_double CpuMonitor::getCoreUsageHistory(
        const unsigned int coreIdx) {
        bool doesCoreExist = this->core_usage_history.size() > coreIdx;
        if (doesCoreExist == false) {
            static const vector_double empty;
            return empty;
        }

        return this->core_usage_history[coreIdx];
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
        catch (const monitor::exceptions::CpuSampleException& e) {
            return;
        }

        if (this->hasSampledOnce == false)
        {
            prevSample = currentSample;
            hasSampledOnce = true;

            latestSnapshot.window_ns = 0;
            latestSnapshot.total_percentage = 0.0;
            latestSnapshot.per_core_percentage.assign(
                currentSample.per_core.size(), 0.0);

            core_usage_history.assign(currentSample.per_core.size(), vector_double());

            return;
        }

        latestSnapshot.window_ns = currentSample.timestamp_ns - prevSample.timestamp_ns;

        // Push Total Cpu Usage into histroy

        double processor_usage = computeTotalPercentage(currentSample, prevSample);

        latestSnapshot.total_percentage = processor_usage;

        this->cpu_usage_history.push_back(processor_usage);


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

            this->core_usage_history[coreIdx].push_back(corePercentage);
        }

        this->prevSample = std::move(currentSample);
    }

};