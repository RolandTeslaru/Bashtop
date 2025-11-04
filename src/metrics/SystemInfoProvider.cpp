#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>
#include <ostream>

#include "monitor/metrics/SystemInfoProvider.hpp"

// Os agnostic info provider

namespace monitor::metrics {
    SystemInfoProvider::SystemInfoProvider(
        std::unique_ptr<monitor::os::AbstractPlatformInfo> platformInfo
    ){
        this->platformInfo = std::move(platformInfo);
    }

    std::ostream& operator<<(std::ostream& os, const SystemInfoProvider& provider) {
        // Basically forward to AbstractPlatformInfo operator<<
        os << *provider.platformInfo;
        return os;
    }
}