#ifndef _WIN32

#error "CpuReaderWin.cpp should only be compiled on Windows."

#endif

#include <windows.h>
#include <winternl.h>
#include <vector>
#include <cstdint>
#include <chrono>
#include <tuple>

#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/types/Cpu.hpp"
#include "monitor/ansi.hpp"

#include "monitor/exceptions/SampleExceptions.hpp"

// The recommended win32 api only exposes total cpu usage, not per core.
// Therefore we use the lower level NT API from ntdll.dll

using CpuRawSample = monitor::types::cpu::RawSample;
using CpuCoreTicks = monitor::types::cpu::CoreTicks;

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::nanoseconds;


namespace monitor::os::win
{
    // Pointer to the query function 
    using NtQueryFunction       = decltype(&::NtQuerySystemInformation);
    using NtCorePerformanceInfo = SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;
    
    class CpuReader final : public monitor::os::AbstractCpuReader
    {
        public:
            void sample(CpuRawSample &out) override
            {
                DWORD logical_cpus = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
                if (!logical_cpus)
                    throw monitor::exceptions::CpuSampleException("Failed to get logical CPU count");

                std::vector<NtCorePerformanceInfo> ntProcessorInfo(logical_cpus);

                static NtQueryFunction ntQuery = loadNtQueryFunction();
                if (!ntQuery)
                    return false;

                ULONG return_length = 0;

                // Query the nt kernel for processor performance info
                NTSTATUS status = ntQuery(
                    SystemProcessorPerformanceInformation, // magic number 8 (processor info)
                    ntProcessorInfo.data(),                // output buffer
                    static_cast<ULONG>(ntProcessorInfo.size() * sizeof(NtCorePerformanceInfo)),
                    &return_length
                );

                if (!NT_SUCCESS(status))
                    throw monitor::exceptions::CpuSampleException("Failed to query nt kernel for processor info");

                readCores(out, logical_cpus, ntProcessorInfo);

                // timestamp
                const auto now = Clock::now().time_since_epoch();
                out.timestamp_ns = static_cast<uint64_t>(
                    std::chrono::duration_cast<Nanoseconds>(now).count());
            }

            void print(std::ostream& os) const override {
                os << monitor::ansi::BOLD << monitor::ansi::BLUE << "CpuReaderWin" << monitor::ansi::RESET << std::endl;
            }
            
        private:

            // loads the NtQuerySystemInformation function from ntdll.dll
            static NtQueryFunction loadNtQueryFunction(){
                // get memory address of the nt query function
                HMODULE ntdll = ::GetModuleHandleW(L"ntdll.dll");
                if (!ntdll)
                    return nullptr;

                FARPROC proc = ::GetProcAddress(ntdll, "NtQuerySystemInformation");
                if (!proc)
                    return nullptr;

                // fix annoying cast warnings
                union {
                    FARPROC         p;
                    NtQueryFunction f;
                } u{ proc };

                return u.f;
            }

            static void readCores(
                CpuRawSample& out,
                const DWORD logical_cpus,
                std::vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> ntAllCoresInfo
            ) {
                out.per_core.clear();
                out.per_core.resize(logical_cpus);

                uint64_t total_idle = 0;
                uint64_t total_all = 0;

                for (DWORD coreIdx = 0; coreIdx < logical_cpus; ++coreIdx){
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

                // Unlike Linux or Mac, windows counts idle time as part of kernel timme.
                const uint64_t kernel_no_idle = (kernel > idle) ? (kernel - idle) : 0ULL;

                const uint64_t busy = kernel_no_idle + user;
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