#pragma once
#include <memory>
#include "monitor/os/AbstractPlatformInfo.hpp"

namespace monitor::metrics {
    class SystemInfoProvider {
        public:
            explicit SystemInfoProvider(std::unique_ptr<monitor::os::AbstractPlatformInfo> platformInfo);
            ~SystemInfoProvider() = default;

            [[maybe_unused]] const monitor::os::AbstractPlatformInfo& getPlatformInfo() const {
                return *platformInfo;
            }

            friend std::ostream& operator<<(std::ostream& os, const SystemInfoProvider& provider);
        
        private:
            std::unique_ptr<monitor::os::AbstractPlatformInfo> platformInfo;
    };
}