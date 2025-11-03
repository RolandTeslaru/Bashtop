#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>

#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/types/Mem.hpp"


namespace monitor::metrics {
    MemMonitor::MemMonitor(std::unique_ptr<monitor::os::AbstractMemReader> reader)
    : memReader(std::move(reader))
    
    {}

    MemMonitor::~MemMonitor(){
        
    }
}