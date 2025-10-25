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
    class PlatformInfoMac final : public monitor::os::AbstractPlatformInfo
    {
    private:
        std::string cpu_name_;
        std::string arch_;
        uint32_t logical_cpus_ = 0;
        uint32_t physical_cpus_ = 0;
        uint64_t mem_total_bytes_ = 0;
        uint64_t page_size_bytes_ = 0;
        std::string os_version_;
        std::string os_build_;
        std::string kernel_release_;
        std::string model_id_;
        std::string hostname_;

    public:
        PlatformInfoMac()
        {
            bool hasCpuName = sysctl_string("machdep.cpu.brand_string", cpu_name_);
            if (hasCpuName == false)
                cpu_name_ = "Apple processor"; // fallback name

            
            arch_           = uname_machine();
            kernel_release_ = uname_release();
            

            // CPU counts
            int32_t lcpu = 0, pcpu = 0;
            if (sysctl_value("hw.logicalcpu",  &lcpu, sizeof lcpu) && lcpu > 0)
                logical_cpus_ = static_cast<uint32_t>(lcpu);
            if (sysctl_value("hw.physicalcpu", &pcpu, sizeof pcpu) && pcpu > 0)
                physical_cpus_ = static_cast<uint32_t>(pcpu);
            if (physical_cpus_ == 0) 
                physical_cpus_ = logical_cpus_;

            // Memory
            uint64_t mem = 0;
            if (sysctl_value("hw.memsize", &mem, sizeof mem)) 
                mem_total_bytes_ = mem;

            int32_t pz = 0;
            if (sysctl_value("hw.pagesize", &pz, sizeof pz) && pz > 0)
                page_size_bytes_ = static_cast<uint64_t>(pz);
            
            // OS version & build
            std::string prod, build;
            sysctl_string("kern.osproductversion", prod);  // e.g., "16.0.1"
            sysctl_string("kern.osversion",        build); // e.g., "25A362"
            os_version_ = prod.empty() ? "macOS" : ("macOS " + prod);
            os_build_   = build;

            // Hostname
            hostname_ = read_hostname();
        }
        ~PlatformInfoMac() override = default;

        std::string cpu_name()        const override { return cpu_name_; }
        std::string arch()            const override { return arch_; }
        uint32_t    logical_cpus()    const override { return logical_cpus_; }
        uint32_t    physical_cpus()   const override { return physical_cpus_; }
        uint64_t    mem_total_bytes() const override { return mem_total_bytes_; }
        uint64_t    page_size_bytes() const override { return page_size_bytes_; }
        std::string os_version()      const override { return os_version_; }
        std::string os_build()        const override { return os_build_; }
        std::string kernel_release()  const override { return kernel_release_; }
        std::string model_id()        const override { return model_id_; }
        std::string hostname()        const override { return hostname_; }

    private: 
        static bool sysctl_string(const char *name, std::string &out){
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



        static std::string uname_machine(){
            struct utsname u{};

            if (uname(&u) == 0)
                return std::string(u.machine);

            return {};
        }


        static bool sysctl_value(const char *key, void* out, size_t out_size){
            size_t len = out_size;
            if(sysctlbyname(key, &out, &len, nullptr, 0) != 0) 
                return false;

            return len == out_size;
        }

        static std::string uname_release(){
            struct utsname u{};

            if (uname(&u) == 0)
                return std::string(u.release);

            return {};
        }


        static std::string read_hostname(){
            std::array<char, 256> buffer{};
            if (gethostname(buffer.data(), buffer.size()) == 0)
                return std::string(buffer.data());
            return {};
        }

    };
};