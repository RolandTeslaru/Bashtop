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

namespace monitor::os::win {
    class MemReader final : public monitor::os::AbstractMemReader {
        public:
            bool sample(monitor::types::mem::RawSample& out) override {
                MEMORYSTATUSEX memInfo;
                memInfo.dwLength = sizeof(MEMORYSTATUSEX);
                
                if (!GlobalMemoryStatusEx(&memInfo)) {
                    return false;
                }

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

                return true;
            }

            void print(std::ostream& os) const override {
                os << monitor::ansi::BOLD << monitor::ansi::BLUE << "MemReaderWin" << monitor::ansi::RESET << std::endl;
            }
    };
}


namespace monitor::os {
    std::unique_ptr<AbstractMemReader> make_mem_reader(){
        return std::make_unique<win::MemReader>();
    }
}