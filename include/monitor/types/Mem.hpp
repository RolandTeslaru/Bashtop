#pragma once
#include <cstdint>

namespace monitor::types::mem {
    struct RawSample {
        uint64_t total;
        uint64_t used;
        uint64_t free;
        
        uint64_t swapTotal;
        uint64_t swapUsed;
        uint64_t swapFree;
    };

    struct Snapshot {
        double used_percentage = 0.0;
        double free_percentage = 0.0;
        uint64_t total = 0;
        uint64_t used = 0;
        uint64_t free = 0;

        double swap_used_percentage = 0.0;
        double swap_free_percentage = 0.0; 
        uint64_t swapTotal = 0;
        uint64_t swapUsed = 0;
        uint64_t swapFree = 0;
    };
}
