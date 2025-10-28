#include <array>
#include <string>
#include <cstdint>
#include <vector>

#include "monitor/os/AbstractPlatformInfo.hpp"

#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>

namespace monitor::os::mac
{
    class PlatformInfoMac final : public monitor::os::AbstractPlatformInfo{
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
            PlatformInfoMac(){
                readSysString("machdep.cpu.brand_string", cpu_name);
 
                this->readFromUnixObj(arch, kernel_release);

                // CPU counts
                int32_t lcpu = 0;
                readSysValue("hw.logicalcpu",  &lcpu, sizeof lcpu);
                logical_cpus = static_cast<uint32_t>(lcpu);

                int32_t pcpu = 0;
                readSysValue("hw.physicalcpu", &pcpu, sizeof pcpu);
                physical_cpus = static_cast<uint32_t>(pcpu);

                if (physical_cpus == 0) 
                    physical_cpus = logical_cpus;

                // Memory
                uint64_t mem = 0;
                readSysValue("hw.memsize", &mem, sizeof mem);
                mem_total_bytes = mem;

                int32_t pz = 0;
                readSysValue("hw.pagesize", &pz, sizeof pz);
                page_size_bytes = static_cast<uint64_t>(pz);


                // OS stuff
                readSysString("kern.osproductversion", os_version);
                readSysString("kern.osversion",        os_build); 

                hostname = readHostname();
            }
            ~PlatformInfoMac() override = default;


            
            PlatformInfoMac(const PlatformInfoMac& macInfo){
                cpu_name        = macInfo.cpu_name;
                arch            = macInfo.arch;
                logical_cpus    = macInfo.logical_cpus;
                physical_cpus   = macInfo.physical_cpus;
                mem_total_bytes = macInfo.mem_total_bytes;
                page_size_bytes = macInfo.page_size_bytes;
                os_version      = macInfo.os_version;
                os_build        = macInfo.os_build;
                kernel_release  = macInfo.kernel_release;
                model_id        = macInfo.model_id;
                hostname        = macInfo.hostname;
            }



            PlatformInfoMac& operator=(const PlatformInfoMac& macInfo){
                if (this == &macInfo)
                    return *this;
                    
                cpu_name        = macInfo.cpu_name;
                arch            = macInfo.arch;
                logical_cpus    = macInfo.logical_cpus;
                physical_cpus   = macInfo.physical_cpus;
                mem_total_bytes = macInfo.mem_total_bytes;
                page_size_bytes = macInfo.page_size_bytes;
                os_version      = macInfo.os_version;
                os_build        = macInfo.os_build;
                kernel_release  = macInfo.kernel_release;
                model_id        = macInfo.model_id;
                hostname        = macInfo.hostname;
                
                return *this;
            }



            std::string cpu_name()        const override { return cpu_name; }
            std::string arch()            const override { return arch; }
            uint32_t    logical_cpus()    const override { return logical_cpus; }
            uint32_t    physical_cpus()   const override { return physical_cpus; }
            uint64_t    mem_total_bytes() const override { return mem_total_bytes; }
            uint64_t    page_size_bytes() const override { return page_size_bytes; }
            std::string os_version()      const override { return os_version; }
            std::string os_build()        const override { return os_build; }
            std::string kernel_release()  const override { return kernel_release; }
            std::string model_id()        const override { return model_id; }
            std::string hostname()        const override { return hostname; }

        private: 
            static void readFromUnixObj(std::string& arch, std::string& kernelRelease){
                struct utsname unixObj{};
                if (uname(&unixObj) == 0){
                    arch = unixObj.machine;
                    kernelRelease = unixObj.release;
                }
            }



            static bool readSysString(const char *name, std::string &out){
                size_t len = 0;
                if (sysctlbyname(name, nullptr, &len, nullptr, 0) != 0 || len == 0)
                    return false;

                std::vector<char> buffer(len);

                if (sysctlbyname(name, buffer.data(), &len, nullptr, 0) != 0)
                    return false;

                // Remove "\0" at the end
                if (len && buffer[len - 1] == '\0')
                    len = len - 1;

                out.assign(buffer.data(), buffer.data() + len);

                return true;
            }





            static bool readSysValue(const char *key, void* out, size_t out_size){
                size_t len = out_size;
                if(sysctlbyname(key, &out, &len, nullptr, 0) != 0) 
                    return false;

                return len == out_size;
            }




            static std::string readHostname(){
                std::array<char, 256> buffer{};
                if (gethostname(buffer.data(), buffer.size()) == 0)
                    return std::string(buffer.data());
                return {};
            }

    };
};