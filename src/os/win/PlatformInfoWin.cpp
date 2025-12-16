#include <array>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <memory>

#include "monitor/os/AbstractPlatformInfo.hpp"
#include "monitor/os/win/PLatformInfoWin.hpp"

namespace monitor::os::win {

PlatformInfoWin::PlatformInfoWin() {}

}

namespace monitor::os {
    // Engine monitors usually take ownership of the readers
    std::unique_ptr<AbstractPlatformInfo> make_platform_info(){
        return std::make_unique<win::PlatformInfoWin>();
    }
}