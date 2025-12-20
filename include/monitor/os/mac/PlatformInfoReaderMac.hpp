#pragma once

#ifdef __APPLE__

#include "monitor/os/AbstractPlatformInfoReader.hpp"
#include <string>

namespace monitor::os::mac {

    class PlatformInfoReaderMac final : public monitor::os::AbstractPlatformInfoReader {
        public:
            PlatformInfoReaderMac();

        private:
            static void readFromUnixObj(std::string& arch, std::string& release);
            static bool readSysString(const char* name, std::string& out);
            static bool readSysValue(const char* name, void* out, size_t outSize);
            static std::string readHostname();
    };

}

#else
#error "PlatformInfoReaderMac.hpp should only be included on macOS (__APPLE__)"
#endif

