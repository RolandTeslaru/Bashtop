#pragma once

#ifdef _WIN32

#include "monitor/os/AbstractPlatformInfoReader.hpp"
#include <cstdint>
#include <string>

namespace monitor::os::win {

    class PlatformInfoReaderWin final : public monitor::os::AbstractPlatformInfoReader {
        private:
            std::string cpu_name = "Unknown";
            std::string cpu_architecture = "Unknown";
            uint32_t    logical_cpus = 0;
            uint32_t    physical_cpus = 0;
            uint64_t    total_memory_bytes = 0;
            uint64_t    page_size_bytes = 0;
            std::string os_version = "Unknown";
            std::string os_build = "Unknown";
            std::string kernel_release = "Unknown";
            std::string model_identifier = "Unknown";
            std::string host_name = "Unknown";

            // Overrides as in the cpp class (kept private there)
            [[maybe_unused]] std::string getCpuName()        const override { return cpu_name; }
            [[maybe_unused]] std::string getArch()           const override { return cpu_architecture; }
            [[maybe_unused]] uint32_t    getLogicalCpus()    const override { return logical_cpus; }
            [[maybe_unused]] uint32_t    getPhysicalCpus()   const override { return physical_cpus; }
            [[maybe_unused]] uint64_t    getMemTotalBytes()  const override { return total_memory_bytes; }
            [[maybe_unused]] uint64_t    getPageSizeBytes()  const override { return page_size_bytes; }
            [[maybe_unused]] std::string getOsVersion()      const override { return os_version; }
            [[maybe_unused]] std::string getOsBuild()        const override { return os_build; }
            [[maybe_unused]] std::string getKernelRelease()  const override { return kernel_release; }
            [[maybe_unused]] std::string getModelId()        const override { return model_identifier; }
            [[maybe_unused]] std::string getHostname()       const override { return host_name; }

        public:
            PlatformInfoReaderWin();
    };

}

#else
#error "PlatformInfoWin.hpp should only be included on Windows (_WIN32)"
#endif

