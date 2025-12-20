#pragma once
#include <memory>
#include "monitor/os/AbstractPlatformInfoReader.hpp"

using AbstractPlatformInfoReader = monitor::os::AbstractPlatformInfoReader;

namespace monitor::metrics {
    class PlatformInfoMonitor {
        public:
            explicit PlatformInfoMonitor(
                std::unique_ptr<AbstractPlatformInfoReader> reader
            );

            ~PlatformInfoMonitor() = default;

            [[maybe_unused]] const AbstractPlatformInfoReader& getPlatformInfo() const {
                return *platformInfo;
            }

            friend std::ostream& operator<<(std::ostream& os, const PlatformInfoMonitor& provider);
        
        private:
            std::unique_ptr<AbstractPlatformInfoReader> platformInfo;
    };
}