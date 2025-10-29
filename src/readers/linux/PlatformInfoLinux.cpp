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
            }



            PlatformInfoLinux(const PlatformInfoLinux& linuxInfo){
                cpu_name        = linuxInfo.cpu_name;
                arch            = linuxInfo.arch;
                logical_cpus    = linuxInfo.logical_cpus;
                physical_cpus   = linuxInfo.physical_cpus;
                mem_total_bytes = linuxInfo.mem_total_bytes;
                page_size_bytes = linuxInfo.page_size_bytes;
                os_version      = linuxInfo.os_version;
                os_build        = linuxInfo.os_build;
                kernel_release  = linuxInfo.kernel_release;
                model_id        = linuxInfo.model_id;
                hostname        = linuxInfo.hostname;
            }



            PlatformInfoLinux& operator=(const PlatformInfoLinux& linuxInfo){
                if (this == &linuxInfo) 
                    return *this;
                cpu_name        = linuxInfo.cpu_name;
                arch            = linuxInfo.arch;
                logical_cpus    = linuxInfo.logical_cpus;
                physical_cpus   = linuxInfo.physical_cpus;
                mem_total_bytes = linuxInfo.mem_total_bytes;
                page_size_bytes = linuxInfo.page_size_bytes;
                os_version      = linuxInfo.os_version;
                os_build        = linuxInfo.os_build;
                kernel_release  = linuxInfo.kernel_release;
                model_id        = linuxInfo.model_id;
                hostname        = linuxInfo.hostname;
                return *this;
            }



            std::string getCpuName()       const override { return cpu_name; }
            std::string getArch()          const override { return arch; }
            uint32_t    getLogicalCpus()   const override { return logical_cpus; }
            uint32_t    getPhysicalCpus()  const override { return physical_cpus; }
            uint64_t    getMemTotalBytes() const override { return mem_total_bytes; }
            uint64_t    getPageSizeBytes() const override { return page_size_bytes; }
            std::string getOsVersion()     const override { return os_version; }
            std::string getOsBuild()       const override { return os_build; }
            std::string getKernelRelease() const override { return kernel_release; }
            std::string getModelId()       const override { return model_id; }
            std::string getHostname()      const override { return hostname; }

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