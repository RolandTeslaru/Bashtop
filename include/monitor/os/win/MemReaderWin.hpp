#pragma once

#ifdef _WIN32

#include "monitor/os/AbstractMemReader.hpp"
#include "monitor/types/Mem.hpp"
#include <ostream>

namespace monitor::os::win {

    class MemReader final : public monitor::os::AbstractMemReader {
        public:
            AbstractMemReader* clone() const override;
            void sample(monitor::types::mem::RawSample& out) override;
            void print(std::ostream& os) const override;
    };

}

#else
#error "MemReaderWin.hpp should only be included on Windows (_WIN32)"
#endif

