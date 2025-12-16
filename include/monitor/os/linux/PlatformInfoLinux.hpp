#pragma once

#ifdef __linux__

#include "monitor/os/AbstractPlatformInfo.hpp"

namespace monitor::os::linux {

    class PlatformInfoLinux final : public monitor::os::AbstractPlatformInfo {
        public:
            PlatformInfoLinux();
        private:
            static void readFromUnixObj(std::string& arch, std::string& kernelRelease);
            static std::string readHostname();
    };

}

#else
#error "PlatformInfoLinux.hpp should only be included on Linux (__linux__)"
#endif