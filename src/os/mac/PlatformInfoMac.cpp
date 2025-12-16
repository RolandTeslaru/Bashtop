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
#include "monitor/os/mac/PlatformInfoMac.hpp"

#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <unistd.h>

namespace monitor::os::mac {

PlatformInfoMac::PlatformInfoMac() {
    PlatformInfoMac::readSysString("machdep.cpu.brand_string", cpu_name);

    PlatformInfoMac::readFromUnixObj(arch, kernel_release);

    int32_t logical = 0;
    if (PlatformInfoMac::readSysValue("hw.logicalcpu", &logical, sizeof logical) && logical > 0)
        logical_cpus = static_cast<uint32_t>(logical);

    int32_t physical = 0;
    if (PlatformInfoMac::readSysValue("hw.physicalcpu", &physical, sizeof physical) && physical > 0)
        physical_cpus = static_cast<uint32_t>(physical);
    else
        physical_cpus = logical_cpus;

    uint64_t memoryBytes = 0;
    if (PlatformInfoMac::readSysValue("hw.memsize", &memoryBytes, sizeof memoryBytes))
        mem_total_bytes = memoryBytes;

    int32_t pageSize = 0;
    if (PlatformInfoMac::readSysValue("hw.pagesize", &pageSize, sizeof pageSize) && pageSize > 0)
        page_size_bytes = static_cast<uint64_t>(pageSize);

    PlatformInfoMac::readSysString("kern.osproductversion", os_version);
    PlatformInfoMac::readSysString("kern.osversion", os_build);
    PlatformInfoMac::readSysString("hw.model", model_id);
    if (model_id.empty()) {
        PlatformInfoMac::readSysString("hw.machine", model_id);
    }

    hostname = PlatformInfoMac::readHostname();
}

void PlatformInfoMac::readFromUnixObj(std::string& archOut, std::string& release) {
    struct utsname info{};
    if (uname(&info) == 0) {
        archOut = info.machine;
        release = info.release;
    }
}

bool PlatformInfoMac::readSysString(const char* name, std::string& out) {
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

bool PlatformInfoMac::readSysValue(const char* name, void* out, size_t outSize) {
    size_t length = outSize;
    if (sysctlbyname(name, out, &length, nullptr, 0) != 0)
        return false;

    return length == outSize;
}

std::string PlatformInfoMac::readHostname() {
    std::array<char, 256> buffer{};
    if (gethostname(buffer.data(), buffer.size()) == 0) {
        return std::string(buffer.data());
    }
    return {};
}

}

namespace monitor::os {
    // Engine monitors usually take ownership of the readers
    std::unique_ptr<AbstractPlatformInfo> make_platform_info() {
        return std::make_unique<mac::PlatformInfoMac>();
    }
}