#ifndef _WIN32

#error "CpuReaderWin.cpp should only be compiled on Windows."

#endif

#include <windows.h>
#include <vector>
#include <cstdint>
#include <chrono>

#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/types/Cpu.hpp"


// NT pieces we need (we keep them in the .cpp so they don't leak to headers)
extern "C" {

// real NT enum is bigger, we only need this one
typedef LONG NTSTATUS;

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG         InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

}

#define SystemProcessorPerformanceInformation 8
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)





using CpuRawSample = monitor::types::cpu::RawSample;
using CpuCoreTicks = monitor::types::cpu::CoreTicks;

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::nanoseconds;


namespace monitor::os::win {
    class CpuReader final : public monitor::os::AbstractCpuReader {
        public:
            bool sample(CpuRawSample& out) override {
                return true;
            }
    };
};



namespace monitor::os {
    std::unique_ptr<AbstractCpuReader> make_cpu_reader(){
        return std::make_unique<win::CpuReader>();
    }
}