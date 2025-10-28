#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <algorithm>

#include "monitor/metrics/SystemInfoProvider.hpp"

namespace monitor::metrics {
    SystemInfoProvider::SystemInfoProvider(
        std::unique_ptr<monitor::os::AbstractPlatformInfo> platformInfo
    ){
        this->platformInfo = std::move(platformInfo);
    }
}