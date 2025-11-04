#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>

#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/types/Mem.hpp"
#include "monitor/ansi.hpp"

namespace monitor::metrics {
    MemMonitor::MemMonitor(std::unique_ptr<monitor::os::AbstractMemReader> reader)
    : memReader(std::move(reader))
    
    {}

    MemMonitor::~MemMonitor(){}

    std::ostream& operator<<(std::ostream& os, const MemMonitor& mon){
        os << monitor::ansi::BOLD << monitor::ansi::CYAN << "MemMonitor: " << monitor::ansi::RESET << std::endl;
        os << "  Reader: "    << *mon.memReader; // deref unique ptr
        return os;
    }
}