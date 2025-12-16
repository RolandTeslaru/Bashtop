#ifdef __linux__

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

#include "monitor/os/linux/CpuReaderLinux.hpp"

using CpuRawSample = monitor::types::cpu::RawSample;
using CpuCoreTicks = monitor::types::cpu::CoreTicks;

using Clock        = std::chrono::steady_clock;
using Nanoseconds  = std::chrono::nanoseconds;


// proc/stat

//      user    system    iowait....
//          nice      idle    irq.....
// 
// cpu  1898 376 1222 29001 97 0 52 0 0 0
// cpu0 896 216 702 14379 52 0 38 0 0 0
// cpu1 1002 160 519 14622 45 0 14 0 0 0
// intr 233505 0 84050 2176 0 0 0 0 0 0 0 56551 0 0 0 0 14452 0 0 0 0 17 0 4076 0 57 111 0 747 0 71268 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
// ctxt 551702
// btime 1761608461
// processes 2989
// procs_running 2
// procs_blocked 0
// softirq 142616 3917 15798 3 289 14960 0 505 37266 0 69878

namespace monitor::os::linux {

AbstractCpuReader* CpuReader::clone() const {
    return new CpuReader(*this);
}

void CpuReader::sample(CpuRawSample& out) {
    std::ifstream f("/proc/stat");
    if (!f) {
        throw monitor::exceptions::CpuSampleException("Failed to open /proc/stat");
    }

    out.per_core.clear();

    std::string line;

    while (std::getline(f, line)) {
        if (!CpuReader::isOnCpuLine(line))
            break;

        std::istringstream iss(line);
        std::string label;

        iss >> label; // total 'cpu' or per-core 'cpuN'

        uint64_t user = 0, nicev = 0, sys = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0, guest_nice = 0;
        iss >> user >> nicev >> sys >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

        const uint64_t idle_ticks = idle + iowait;
        const uint64_t non_idle_ticks = user + nicev + sys + irq + softirq + steal;
        const uint64_t all_ticks = idle_ticks + non_idle_ticks;

        if (label == "cpu") {
            out.total.idle = idle_ticks;
            out.total.total = all_ticks;
        } else if (label.rfind("cpu", 0) == 0) {
            const int coreIdx = std::stoi(label.substr(3));
            if (coreIdx >= 0) {
                if (static_cast<size_t>(coreIdx) >= out.per_core.size())
                    out.per_core.resize(coreIdx + 1);

                out.per_core[coreIdx].idle = idle_ticks;
                out.per_core[coreIdx].total = all_ticks;
            }
        } else {
            break;
        }
    }

    const auto now = Clock::now().time_since_epoch();
    out.timestamp_ns = this->toNanoseconds(now);
}

void CpuReader::print(std::ostream& os) const {
    os << monitor::ansi::BOLD << monitor::ansi::BLUE << "CpuReaderLinux: " << monitor::ansi::RESET << std::endl;
}

bool CpuReader::isOnCpuLine(const std::string& line) {
    if (line.rfind("cpu", 0) != 0)
        return false;
    return true;
}

}


namespace monitor::os {
    // Engine monitors usually take ownership of the readers
    std::unique_ptr<AbstractCpuReader> make_cpu_reader(){
        return std::make_unique<linux::CpuReader>();
    }
}

#endif