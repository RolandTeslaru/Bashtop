#pragma once
#include <vector>
#include <ostream>
#include <string>
#include <cstdint>

namespace monitor::os {
    class AbstractPlatformInfo {
        public:
            virtual ~AbstractPlatformInfo() = default;

            // CPU
            virtual std::string getCpuName()        const = 0; 
            virtual std::string getArch()            const = 0; 
            virtual uint32_t    getLogicalCpus()    const = 0;
            virtual uint32_t    getPhysicalCpus()   const = 0;

            // Memory
            virtual uint64_t    getMemTotalBytes() const = 0;
            virtual uint64_t    getPageSizeBytes() const = 0;

            // OS / device stuff
            virtual std::string getOsVersion()      const = 0;
            virtual std::string getOsBuild()        const = 0;
            virtual std::string getKernelRelease()  const = 0;
            virtual std::string getModelId()        const = 0;
            virtual std::string getHostname()        const = 0;

            friend std::ostream& operator<<(std::ostream& os, const AbstractPlatformInfo& absInfo){
                os << "Platform Info: "   << std::endl
                   << "  CPU: "           << absInfo.getCpuName() << std::endl
                   << "  Architecture: "  << absInfo.getArch() << std::endl
                   << "  Logical CPUs: "  << absInfo.getLogicalCpus() << std::endl
                   << "  Physical CPUs: " << absInfo.getPhysicalCpus() << std::endl
                   << "  Memory: "        << absInfo.getMemTotalBytes() << std::endl
                   << "  Page Size: "     << absInfo.getPageSizeBytes() << std::endl
                   << "  OS Version: "    << absInfo.getOsVersion() << std::endl
                   << "  OS Build: "      << absInfo.getOsBuild() << std::endl
                   << "  Kernel: "        << absInfo.getKernelRelease() << std::endl
                   << "  Model ID: "      << absInfo.getModelId() << std::endl
                   << "  Hostname: "      << absInfo.getHostname();
                return os;
            }
    };
}