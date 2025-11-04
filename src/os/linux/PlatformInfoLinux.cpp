#include <array>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <memory>

#include "monitor/os/AbstractPlatformInfo.hpp"


#include <unistd.h>
#include <sys/utsname.h>


namespace monitor::os::linux 
{
    class PlatformInfoLinux final : public monitor::os::AbstractPlatformInfo{
        public:
            PlatformInfoLinux(){
                this->readFromUnixObj(arch, kernel_release);

                // CPU counts
                long onln = sysconf(_SC_NPROCESSORS_ONLN);
                if (onln > 0) 
                    logical_cpus = static_cast<uint32_t>(onln);
                physical_cpus = logical_cpus;

                // Memory
                long pages = sysconf(_SC_PHYS_PAGES);
                long psz   = sysconf(_SC_PAGESIZE);
                if (pages > 0 && psz > 0)
                    mem_total_bytes = static_cast<uint64_t>(pages) * static_cast<uint64_t>(psz);
                if (psz > 0)
                    page_size_bytes = static_cast<uint64_t>(psz);

                os_version = "Linux";

                hostname = readHostname();


                // TODO: read cpu_name, os_build, model_id and other
            }
            
        private:
            static void readFromUnixObj(std::string& arch, std::string& kernelRelease){
                struct utsname unixObj{};
                if (uname(&unixObj) == 0){
                    arch = unixObj.machine;
                    kernelRelease = unixObj.release;
                }
            }



            static std::string readHostname(){
                std::array<char, 256> buffer{};
                if (gethostname(buffer.data(), buffer.size()) == 0)
                    return std::string(buffer.data());
                return {};
            }
    };

}


namespace monitor::os {
    std::unique_ptr<AbstractPlatformInfo> make_platform_info(){
        return std::make_unique<linux::PlatformInfoLinux>();
    }
}