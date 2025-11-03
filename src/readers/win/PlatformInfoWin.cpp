#include <array>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <memory>

#include "monitor/os/AbstractPlatformInfo.hpp"


#include <unistd.h>
#include <sys/utsname.h>


namespace monitor::os::win
{
    class PlatformInfoWin final : public monitor::os::AbstractPlatformInfo{
        private:
            std::string cpu_name;
            std::string arch;
            uint32_t    logical_cpus    = 0;
            uint32_t    physical_cpus   = 0;
            uint64_t    mem_total_bytes = 0;
            uint64_t    page_size_bytes = 0;
            std::string os_version;
            std::string os_build;
            std::string kernel_release;
            std::string model_id;
            std::string hostname;

        public:
            PlatformInfoWin(){}
    };
}

namespace monitor::os {
    std::unique_ptr<AbstractPlatformInfo> make_platform_info(){
        return std::make_unique<win::PlatformInfoWin>();
    }
}