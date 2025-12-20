#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>
#include <ostream>

#include "monitor/metrics/PlatformInfoMonitor.hpp"

// Os agnostic info provider

using AbstractPlatformInfoReader = monitor::os::AbstractPlatformInfoReader;

namespace monitor::metrics {
    PlatformInfoMonitor::PlatformInfoMonitor(
        std::unique_ptr<AbstractPlatformInfoReader> reader
    ){
        this->platformInfo = std::move(reader);
    }

    std::ostream& operator<<(std::ostream& os, const PlatformInfoMonitor& monitor) {
        // Basically forward to AbstractPlatformInfoReader operator<<
        os << *monitor.platformInfo;
        return os;
    }
}