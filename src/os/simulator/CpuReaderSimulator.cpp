
#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <cctype>
#include <iostream>

#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/types/Cpu.hpp"
#include "monitor/ansi.hpp"
#include "monitor/exceptions/SampleExceptions.hpp"

#include "monitor/os/simulator/CpuReaderSimulator.hpp"
#include <numeric> 

using CpuRawSample = monitor::types::cpu::RawSample;
using CpuCoreTicks = monitor::types::cpu::CoreTicks;

using Clock        = std::chrono::steady_clock;
using Nanoseconds  = std::chrono::nanoseconds;


namespace monitor::os::simulator {

    using CpuSampleException = monitor::exceptions::CpuSampleException;

    AbstractCpuReader* CpuReader::clone() const {
        return new CpuReader(*this);
    }

    void CpuReader::sample(CpuRawSample& out) {
        // simulate a CPU with 4 cores
        const int num_cores = 4;
        
        static std::vector<uint64_t> idle(num_cores, 100000);
        static std::vector<uint64_t> total(num_cores, 200000);

        out.per_core.assign(num_cores, CpuCoreTicks{});

        // Simulate some CPU activity
        for (int i = 0; i < num_cores; ++i) {
            idle[i]  += 1000 + (i * 500);
            total[i] += 5000 + (i * 1000);

            out.per_core[i].idle  = idle[i];
            out.per_core[i].total = total[i];
        }

        out.total.idle  = std::accumulate(idle.begin(), idle.end(), uint64_t{0});
        out.total.total = std::accumulate(total.begin(), total.end(), uint64_t{0});

        const auto now = Clock::now().time_since_epoch();
        out.timestamp_ns = this->toNanoseconds(now);
    }

    void CpuReader::print(std::ostream& os) const {
        os << monitor::ansi::BOLD << monitor::ansi::BLUE << "CpuReaderSimulator: " << monitor::ansi::RESET << std::endl;
    }
}