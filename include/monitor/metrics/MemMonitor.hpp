#pragma once
#include <memory>
#include <ostream>
#include "monitor/os/AbstractMemReader.hpp"

namespace monitor::metrics {
    class MemMonitor {
        public:
            explicit MemMonitor(std::unique_ptr<monitor::os::AbstractMemReader> reader);
            ~MemMonitor();

            friend std::ostream& operator<<(std::ostream& os, const MemMonitor& mon);

        private:
            std::unique_ptr<monitor::os::AbstractMemReader> memReader;
    };
}