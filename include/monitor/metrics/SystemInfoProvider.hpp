#include <memory>
#include "monitor/os/AbstractPlatformInfo.hpp"

namespace monitor::metrics {
    class SystemInfoProvider {
        public:
            SystemInfoProvider(std::unique_ptr<monitor::os::AbstractPlatformInfo> platformInfo);
            ~SystemInfoProvider() = default;

            const monitor::os::AbstractPlatformInfo& getPlatformInfo() const {
                return *platformInfo;
            }
        
        private:
            std::unique_ptr<monitor::os::AbstractPlatformInfo> platformInfo;
    };
}