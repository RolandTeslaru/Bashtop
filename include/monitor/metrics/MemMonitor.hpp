#pragma once
#include <memory>
#include <ostream>
#include "monitor/os/AbstractMemReader.hpp"

namespace monitor::metrics {
    class MemMonitor {
        public:
            explicit MemMonitor(std::unique_ptr<monitor::os::AbstractMemReader> reader);
            ~MemMonitor();

        private:
            std::unique_ptr<monitor::os::AbstractMemReader> memReader;
    };
}