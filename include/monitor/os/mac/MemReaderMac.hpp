#pragma once

#ifdef __APPLE__

#include "monitor/os/AbstractMemReader.hpp"
#include "monitor/types/Mem.hpp"
#include <ostream>

namespace monitor::os::mac {

    class MemReader final : public monitor::os::AbstractMemReader {
        public:
            void sample(monitor::types::mem::RawSample& out) override;
            void print(std::ostream& os) const override;
    };

}

#else
#error "MemReaderMac.hpp should only be included on macOS (__APPLE__)"
#endif

