#pragma once

#ifdef __APPLE__

#include "monitor/os/AbstractPlatformInfo.hpp"
#include <string>

namespace monitor::os::mac {

    class PlatformInfoMac final : public monitor::os::AbstractPlatformInfo {
        public:
            PlatformInfoMac();

        private:
            static void readFromUnixObj(std::string& arch, std::string& release);
            static bool readSysString(const char* name, std::string& out);
            static bool readSysValue(const char* name, void* out, size_t outSize);
            static std::string readHostname();
    };

}

#else
#error "PlatformInfoMac.hpp should only be included on macOS (__APPLE__)"
#endif

