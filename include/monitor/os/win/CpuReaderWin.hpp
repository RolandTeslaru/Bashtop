#pragma once

#ifdef _WIN32

#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/types/Cpu.hpp"
#include <ostream>
#include <vector>

#include <windows.h>
#include <winternl.h>

namespace monitor::os::win {

    // Pointer to the query function (same alias as in cpp)
    using NtQueryFunction       = decltype(&::NtQuerySystemInformation);
    using NtCorePerformanceInfo = SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

    class CpuReader final : public monitor::os::AbstractCpuReader {
        public:
            AbstractCpuReader* clone() const override;
            void sample(monitor::types::cpu::RawSample& out) override;
            void print(std::ostream& os) const override;

        private:
            static NtQueryFunction loadNtQueryFunction();
            static void readCores(
                monitor::types::cpu::RawSample& out,
                const DWORD logical_cpus,
                std::vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> ntAllCoresInfo);
            static std::tuple<const uint64_t, const uint64_t> readPerCoreTicks(
                monitor::types::cpu::CoreTicks& coreTicks,
                const SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION& ntCoreInfo);
    };

}

#else
#error "CpuReaderWin.hpp should only be included on Windows (_WIN32)"
#endif

