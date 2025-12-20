#ifdef _WIN32

#include <array>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <memory>

#include "monitor/os/AbstractPlatformInfoReader.hpp"
#include "monitor/os/win/PlatformInfoReaderWin.hpp"

namespace monitor::os::win {

PlatformInfoReaderWin::PlatformInfoReaderWin() {}

}

namespace monitor::os {
    // Engine monitors usually take ownership of the readers
    std::unique_ptr<AbstractPlatformInfoReader> make_platform_info_reader(){
        return std::make_unique<win::PlatformInfoReaderWin>();
    }
}

#endif