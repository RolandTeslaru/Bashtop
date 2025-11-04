#include <array>
#include <string>
#include <cstdint>
#include <vector>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "monitor/os/AbstractPlatformInfo.hpp"

#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <unistd.h>

namespace monitor::os::mac {

class PlatformInfoMac final : public monitor::os::AbstractPlatformInfo {
    public:
        PlatformInfoMac() {
            readSysString("machdep.cpu.brand_string", cpu_name);

            readFromUnixObj(arch, kernel_release);

            int32_t logical = 0;
            if (readSysValue("hw.logicalcpu", &logical, sizeof logical) && logical > 0)
                logical_cpus = static_cast<uint32_t>(logical);

            int32_t physical = 0;
            if (readSysValue("hw.physicalcpu", &physical, sizeof physical) && physical > 0)
                physical_cpus = static_cast<uint32_t>(physical);
            else
                physical_cpus = logical_cpus;

            uint64_t memoryBytes = 0;
            if (readSysValue("hw.memsize", &memoryBytes, sizeof memoryBytes))
                mem_total_bytes = memoryBytes;

            int32_t pageSize = 0;
            if (readSysValue("hw.pagesize", &pageSize, sizeof pageSize) && pageSize > 0)
                page_size_bytes = static_cast<uint64_t>(pageSize);


            readSysString("kern.osproductversion", os_version);
            readSysString("kern.osversion", os_build);
            readSysString("hw.model", model_id);
            if (model_id.empty()) {
                readSysString("hw.machine", model_id);
            }

            hostname = readHostname();
        }

    private:
        static void readFromUnixObj(std::string &arch, std::string &release) {
            struct utsname info {};
            if (uname(&info) == 0) {
                arch = info.machine;
                release = info.release;
            }
        }

        static bool readSysString(const char *name, std::string &out) {
            size_t length = 0;
            if (sysctlbyname(name, nullptr, &length, nullptr, 0) != 0 || length == 0)
                return false;

            std::vector<char> buffer(length);
            if (sysctlbyname(name, buffer.data(), &length, nullptr, 0) != 0)
                return false;

            if (!buffer.empty() && buffer.back() == '\0')
                buffer.pop_back();

            out.assign(buffer.begin(), buffer.end());
            return true;
        }

        static bool readSysValue(const char *name, void *out, size_t outSize) {
            size_t length = outSize;
            if (sysctlbyname(name, out, &length, nullptr, 0) != 0)
                return false;
                
            return length == outSize;
        }

        static std::string readHostname() {
            std::array<char, 256> buffer{};
            if (gethostname(buffer.data(), buffer.size()) == 0) {
                return std::string(buffer.data());
            }
            return {};
        }
    };

}

namespace monitor::os {
    std::unique_ptr<AbstractPlatformInfo> make_platform_info() {
        return std::make_unique<mac::PlatformInfoMac>();
    }
}