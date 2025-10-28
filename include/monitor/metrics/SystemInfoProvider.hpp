#include <memory>
#include "monitor/os/AbstractPlatformInfo.hpp"

namespace monitor::metrics {
    class SystemInfoProvider {
        public:
            SystemInfoProvider(std::unique_ptr<monitor::os::AbstractPlatformInfo> platformInfo);
            ~SystemInfoProvider() = default;
        
        private:
            std::unique_ptr<monitor::os::AbstractPlatformInfo> platformInfo;
    };
}