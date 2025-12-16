#pragma once

#ifdef __APPLE__

#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/types/Cpu.hpp"
#include <ostream>

// macos specifc includes
#include <mach/mach_host.h>
#include <mach/processor_info.h>
#include <mach/mach.h>

namespace monitor::os::mac {

    class CpuReader final : public monitor::os::AbstractCpuReader {
        public:
            AbstractCpuReader* clone() const override;
            void sample(monitor::types::cpu::RawSample& out) override;
            void print(std::ostream& os) const override;

        private:
            static void readKernelLoads(
                processor_info_array_t info,
                natural_t cpuCount,
                monitor::types::cpu::RawSample& out);
    };

}

#else
#error "CpuReaderMac.hpp should only be included on macOS (__APPLE__)"
#endif

