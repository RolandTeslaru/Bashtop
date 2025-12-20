#pragma once

#ifdef __linux__

#include "monitor/os/AbstractPlatformInfoReader.hpp"

namespace monitor::os::linux {

    class PlatformInfoReaderLinux final : public monitor::os::AbstractPlatformInfoReader {
        public:
            PlatformInfoReaderLinux();
        private:
            static void readFromUnixObj(std::string& arch, std::string& kernelRelease);
            static std::string readHostname();
    };

}

#else
#error "PlatformInfoReaderLinux.hpp should only be included on Linux (__linux__)"
#endif