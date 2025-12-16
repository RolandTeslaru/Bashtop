#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <cctype>
#include <iostream>

#include "monitor/os/AbstractMemReader.hpp"
#include "monitor/ansi.hpp"
#include "monitor/exceptions/SampleExceptions.hpp"

#include "monitor/os/linux/MemReaderLinux.hpp"

namespace monitor::os::linux {

void MemReader::sample(monitor::types::mem::RawSample& out) {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open())
        throw monitor::exceptions::MemSampleException("Failed to open memInfo");

    std::string line;
    std::string key;
    uint64_t value;
    std::string unit;

    uint64_t memTotal = 0;
    uint64_t memFree = 0;
    uint64_t swapTotal = 0;
    uint64_t swapFree = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ss >> key >> value >> unit;

        if (key == "MemTotal:") memTotal = value;
        else if (key == "MemFree:") memFree = value;
        else if (key == "SwapTotal:") swapTotal = value;
        else if (key == "SwapFree:") swapFree = value;
    }

    // Convert kB to Bytes
    out.total = memTotal * 1024;
    out.free = memFree * 1024;
    // Consistent with Mac implementation: Used = Total - Free (includes cache)
    out.used = (memTotal - memFree) * 1024;

    out.swapTotal = swapTotal * 1024;
    out.swapFree = swapFree * 1024;
    out.swapUsed = (swapTotal - swapFree) * 1024;
}

void MemReader::print(std::ostream& os) const {
    os << monitor::ansi::BOLD << monitor::ansi::BLUE << "MemReaderLinux" << monitor::ansi::RESET << std::endl;
}

}

namespace monitor::os {
        // Engine monitors usually take ownership of the readers
    std::unique_ptr<AbstractMemReader> make_mem_reader()
    {
        return std::make_unique<linux::MemReader>();
    }
}