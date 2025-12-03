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
}
