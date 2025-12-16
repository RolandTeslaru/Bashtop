#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <cctype>
#include <iostream>

#include <windows.h>

#include "monitor/os/AbstractMemReader.hpp"
#include "monitor/ansi.hpp"
#include "monitor/exceptions/SampleExceptions.hpp"

#include "monitor/os/win/MemReaderWin.hpp"

namespace monitor::os::win {

void MemReader::sample(monitor::types::mem::RawSample& out) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (!GlobalMemoryStatusEx(&memInfo))
        throw monitor::exceptions::MemSampleException("Failed to get memory status");

    // Physical Memory
    out.total = memInfo.ullTotalPhys;
    out.free = memInfo.ullAvailPhys;
    out.used = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

    // swap (pageFile) approximation
    // ullTotalPageFile = Commit Limit (Physical + PageFile)
    // ullAvailPageFile = Available Commit

    uint64_t commitLimit = memInfo.ullTotalPageFile;
    uint64_t commitAvail = memInfo.ullAvailPageFile;
    uint64_t commitUsed = commitLimit - commitAvail;

    // Approximate PageFile size on disk
    if (commitLimit > out.total) {
        out.swapTotal = commitLimit - out.total;
    } else {
        out.swapTotal = 0;
    }

    // Used Swap = Used Commit - Used RAM
    if (commitUsed > out.used) {
        out.swapUsed = commitUsed - out.used;
    } else {
        out.swapUsed = 0;
    }

    if (out.swapTotal > out.swapUsed) {
        out.swapFree = out.swapTotal - out.swapUsed;
    } else {
        out.swapFree = 0;
    }
}

void MemReader::print(std::ostream& os) const {
    os << monitor::ansi::BOLD << monitor::ansi::BLUE << "MemReaderWin" << monitor::ansi::RESET << std::endl;
}

}


namespace monitor::os {
    // Engine monitors usually take ownership of the readers
    std::unique_ptr<AbstractMemReader> make_mem_reader(){
        return std::make_unique<win::MemReader>();
    }
}