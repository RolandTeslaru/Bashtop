#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>
#include <iostream>

#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/types/Mem.hpp"
#include "monitor/ansi.hpp"
#include "monitor/exceptions/SampleExceptions.hpp"


using RawSample = monitor::types::mem::RawSample;

namespace monitor::metrics {
    using MemSampleException = monitor::exceptions::MemSampleException;

    // ============================================================================
    // Constructor / Destructor / Copy Constructor
    // ============================================================================
    MemMonitor::MemMonitor(
        std::unique_ptr<monitor::os::AbstractMemReader> reader
    ): memReader(std::move(reader)) {
        RawSample sample;
        memReader->sample(sample);
    }

    MemMonitor::~MemMonitor() {}

    MemMonitor::MemMonitor(const MemMonitor& other)
        : memReader(other.memReader->clone()),
          hasSampledOnce(other.hasSampledOnce),
          prevSample(other.prevSample),
          latestSnapshot(other.latestSnapshot)
    {}

    // ============================================================================
    // Operator Overloads
    // ============================================================================
    std::ostream& operator<<(std::ostream& os, const MemMonitor& mon) {
        os << monitor::ansi::BOLD << monitor::ansi::CYAN << "MemMonitor: " << monitor::ansi::RESET << std::endl;
        os << "  Reader: " << *mon.memReader; // deref unique ptr
        return os;
    }

    MemMonitor& MemMonitor::operator=(MemMonitor other){
        swap(*this, other);
        return *this;
    }

    void swap(MemMonitor& Mon1, MemMonitor& Mon2) noexcept{
        std::swap(Mon1.memReader, Mon2.memReader);
        std::swap(Mon1.prevSample, Mon2.prevSample);
        std::swap(Mon1.latestSnapshot, Mon2.latestSnapshot);
        std::swap(Mon1.hasSampledOnce, Mon2.hasSampledOnce);
    }

    // ============================================================================
    // Public Interface
    // ============================================================================
    void MemMonitor::computeSnapshot() {
        RawSample currentSample{};

        try {
            this->memReader->sample(currentSample);
        } catch (const MemSampleException& e) {
            std::cerr << "Memory sampling error: " << e.what() << std::endl;
            throw; 
        }

        this->memReader->sample(currentSample);

        if (this->hasSampledOnce == false) {
            prevSample = currentSample;
            hasSampledOnce = true;
        }

        latestSnapshot.total = currentSample.total;
        latestSnapshot.used = currentSample.used;
        latestSnapshot.free = currentSample.free;

        if (currentSample.total != 0) {
            latestSnapshot.used_percentage = (static_cast<double>(currentSample.used) / static_cast<double>(currentSample.total)) * 100.0;
            latestSnapshot.free_percentage = (static_cast<double>(currentSample.free) / static_cast<double>(currentSample.total)) * 100.0;
        } else {
            latestSnapshot.used_percentage = 0.0;
            latestSnapshot.free_percentage = 0.0;
        }   

        latestSnapshot.swapTotal = currentSample.swapTotal;
        latestSnapshot.swapUsed = currentSample.swapUsed;
        latestSnapshot.swapFree = currentSample.swapFree;

        if (currentSample.swapTotal != 0) {
            latestSnapshot.swap_used_percentage = (static_cast<double>(currentSample.swapUsed) / static_cast<double>(currentSample.swapTotal)) * 100.0;
            latestSnapshot.swap_free_percentage = (static_cast<double>(currentSample.swapFree) / static_cast<double>(currentSample.swapTotal)) * 100.0;
        } else {
            latestSnapshot.swap_used_percentage = 0.0;
            latestSnapshot.swap_free_percentage = 0.0;
        }

        this->prevSample = std::move(currentSample);
    }

    const MemSnapshot& MemMonitor::getLatestSnapshot() const {
        return latestSnapshot;
    }
}