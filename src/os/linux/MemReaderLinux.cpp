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

#include "monitor/os/AbstractMemReader.hpp"
#include "monitor/ansi.hpp"
#include "monitor/exceptions/SampleExceptions.hpp"

#include "monitor/os/linux/MemReaderLinux.hpp"

namespace monitor::os::linux {

    using MemRawSample       = monitor::types::mem::RawSample;
    using MemSampleException = monitor::exceptions::MemSampleException;

    AbstractMemReader* MemReader::clone() const {
        return new MemReader(*this);
    }

    void MemReader::sample(MemRawSample& out) {
        std::ifstream file("/proc/meminfo");
        if (!file.is_open())
            throw MemSampleException("Failed to open /proc/meminfo");

        std::string line;
        std::string key;
        uint64_t value;
        std::string unit;

        uint64_t memTotal = 0;
        uint64_t memFree = 0;
        uint64_t swapTotal = 0;
        uint64_t swapFree = 0;

        bool foundMemTotal = false;
        bool foundMemFree = false;

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            if (!(ss >> key >> value >> unit))
                continue;

            if (key == "MemTotal:") {
                memTotal = value;
                foundMemTotal = true;
            } else if (key == "MemFree:") {
                memFree = value;
                foundMemFree = true;
            } else if (key == "SwapTotal:") {
                swapTotal = value;
            } else if (key == "SwapFree:") {
                swapFree = value;
            }
        }

        if (!foundMemTotal || !foundMemFree)
            throw MemSampleException("Required memory fields (MemTotal/MemFree) not found in /proc/meminfo");

        if (file.bad())
            throw MemSampleException("Error occurred while reading /proc/meminfo");

        // convert kb to bytes
        out.total = memTotal * 1024;
        out.free = memFree * 1024;

        // used = total - free
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

#endif