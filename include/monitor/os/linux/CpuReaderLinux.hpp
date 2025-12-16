#pragma once

#ifdef __linux__


#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/types/Cpu.hpp"
#include <ostream>
#include <string>

namespace monitor::os::linux {
    class CpuReader final : public monitor::os::AbstractCpuReader {
    public:
        AbstractCpuReader* clone() const override;

        void sample(monitor::types::cpu::RawSample& out) override;

        void print(std::ostream& os) const override;

    private:
        static bool isOnCpuLine(const std::string& line);
    };

}


#else
#error "CpuReaderLinux.hpp should only be included on Linux (__linux__)"
#endif