#include <vector>
#include <cstdint>
#include <chrono>

#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/types/Cpu.hpp"


using CpuRawSample = monitor::types::cpu::RawSample;
using CpuCoreTicks = monitor::types::cpu::CoreTicks;

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::nanoseconds;


namespace monitor::os::linux {
    class CpuReader final : public monitor::os::AbstractCpuReader {
        public:
            bool sample(CpuRawSample& out) override {
                return true; // Return false on failure
            }
    };
};


namespace monitor::os {
    std::unique_ptr<AbstractCpuReader> make_cpu_reader(){
        return std::make_unique<linux::CpuReader>();
    }
}