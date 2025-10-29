#pragma once
#include <cstdint>
#include <vector>

namespace monitor::types::cpu {
    struct CoreTicks {
        uint64_t idle  = 0; // ticks spent idle since boot
        uint64_t total = 0; // ticks spent total since boot (idle + busy)
    };
    
    // One read from what the os provides 
    struct RawSample {
        uint64_t                timestamp_ns = 0;
        CoreTicks               total;
        std::vector<CoreTicks>  per_core;
    };
    
    
    // CpuSnapshot is computed from 2 CpuRawSamples: s1 and s2
    struct Snapshot {
        uint64_t             window_ns             = 0; // time between s1 and s2
        double               total_percentage      = 0; // total cpu usage for all cores combined (0 -> 100)
        std::vector<double>  per_core_percentage;       // cpu urage for each core individually   (0 -> 100)
    };
};    
