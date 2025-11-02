#ifndef _WIN32

#error "CpuReaderWin.cpp should only be compiled on Windows."

#endif

#include <windows.h>
#include <vector>
#include <cstdint>
#include <chrono>
#include <tuple>

#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/types/Cpu.hpp"

// The recommended win32 api only exposes total cpu usage, not per core.
// Therefore we use the lower level NT API which is not exposed thru a header file.
// We need to recreate the actual data structres and get the memmory adress of the function that fetches the kernel info ourselves.

#define SystemProcessorPerformanceInformation 8 // specifc number for Processor performance info
// cool facts:
//  5 is the system process info
//  2 is system performance info
// 13 is system time info

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)


extern "C" // use C linkage because nt is written in C
{
    typedef LONG NTSTATUS;
    
    // Structures for the query response
    typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
    {
        LARGE_INTEGER IdleTime;
        LARGE_INTEGER KernelTime;
        LARGE_INTEGER UserTime;
        LARGE_INTEGER DpcTime;
        LARGE_INTEGER InterruptTime;
        ULONG InterruptCount;
    } SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;
    
}

using NtQuerySysInfoFn = NTSTATUS(WINAPI *)(ULONG, PVOID, ULONG, PULONG);
using NtCorePerformanceInfo = SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;



using CpuRawSample = monitor::types::cpu::RawSample;
using CpuCoreTicks = monitor::types::cpu::CoreTicks;

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::nanoseconds;


namespace monitor::os::win
{
    class CpuReader final : public monitor::os::AbstractCpuReader
    {
    public:
        bool sample(CpuRawSample &out) override
        {
            DWORD logical_cpus = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
            if (!logical_cpus)
                return false;

            std::vector<NtCorePerformanceInfo> ntProcessorInfo(logical_cpus);

            // Get the memory address of the hidden NtQuerySystemInformation function
            static NtQuerySysInfoFn ntQuery = reinterpret_cast<NtQuerySysInfoFn>(
                    ::GetProcAddress(
                        ::GetModuleHandleW(L"ntdll.dll"),
                        "NtQuerySystemInformation"
                    )
                );

            if (!ntQuery)
                return false;

            ULONG return_length = 0;

            // Call the hidden function via the function pointer
            NTSTATUS status = ntQuery(
                SystemProcessorPerformanceInformation, // magic number 8 (processor info)
                ntProcessorInfo.data(),                // output buffer
                static_cast<ULONG>(ntProcessorInfo.size() * sizeof(NtCorePerformanceInfo)),
                &return_length
            );

            if (!NT_SUCCESS(status))
                return false;

            readCores(out, logical_cpus, ntProcessorInfo);

            // timestamp
            const auto now = Clock::now().time_since_epoch();
            out.timestamp_ns = static_cast<uint64_t>(
                std::chrono::duration_cast<Nanoseconds>(now).count());

            return true;
        }
    private:
        static void readCores(
            CpuRawSample& out,
            const DWORD logical_cpus,
            std::vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> ntAllCoresInfo
        ) {
            out.per_core.clear();
            out.per_core.resize(logical_cpus);

            uint64_t total_idle = 0;
            uint64_t total_all = 0;

            for (DWORD coreIdx = 0; coreIdx < logical_cpus; ++coreIdx)
            {
                const auto &ntCoreInfo = ntAllCoresInfo[coreIdx];

                const auto [idle, all] = readPerCoreTicks(out.per_core[coreIdx], ntCoreInfo);

                total_idle += idle;
                total_all += all;
            }

            out.total.idle = total_idle;
            out.total.total = total_all;
        }

        static std::tuple<const uint64_t, const uint64_t> readPerCoreTicks(
            monitor::types::cpu::CoreTicks&                 coreTicks, 
            const SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION& ntCoreInfo 
        ){
            const uint64_t idle      = static_cast<uint64_t>(ntCoreInfo.IdleTime.QuadPart);
            const uint64_t kernel    = static_cast<uint64_t>(ntCoreInfo.KernelTime.QuadPart);
            const uint64_t user      = static_cast<uint64_t>(ntCoreInfo.UserTime.QuadPart);
            const uint64_t dpc       = static_cast<uint64_t>(ntCoreInfo.DpcTime.QuadPart);
            const uint64_t interrupt = static_cast<uint64_t>(ntCoreInfo.InterruptTime.QuadPart);

            // Unlike Linux or Mac, windows counts idle time as part of kernel timme.
            const uint64_t kernel_no_idle = (kernel > idle) ? (kernel - idle) : 0ULL;

            const uint64_t busy = kernel_no_idle + user + dpc + interrupt;
            const uint64_t all = busy + idle;

            coreTicks.idle = idle;
            coreTicks.total = all;

            return std::make_tuple(idle, all);
        }
    };


};

namespace monitor::os
{
    std::unique_ptr<AbstractCpuReader> make_cpu_reader()
    {
        return std::make_unique<win::CpuReader>();
    }
}