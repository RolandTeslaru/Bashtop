#include <memory>
#include <ostream>
#include "monitor/os/AbstractMemReader.hpp"

namespace monitor::metrics {
    class MemMonitor {
        public:
            MemMonitor(std::unique_ptr<monitor::os::AbstractMemReader> reader);
            ~MemMonitor();
    };
}