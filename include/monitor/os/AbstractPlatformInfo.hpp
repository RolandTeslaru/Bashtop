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
            virtual std::string cpu_name()        const = 0; 
            virtual std::string arch()            const = 0; 
            virtual uint32_t    logical_cpus()    const = 0;
            virtual uint32_t    physical_cpus()   const = 0;

            // Memory
            virtual uint64_t    mem_total_bytes() const = 0;
            virtual uint64_t    page_size_bytes() const = 0;

            // OS / device stuff
            virtual std::string os_version()      const = 0;
            virtual std::string os_build()        const = 0;
            virtual std::string kernel_release()  const = 0;
            virtual std::string model_id()        const = 0;
            virtual std::string hostname()        const = 0;       
            
            friend std::ostream& operator<<(std::ostream& os, const AbstractPlatformInfo& absInfo){
                os << "Platform Info: "   << std::endl
                   << "  CPU: "           << absInfo.cpu_name() << std::endl
                   << "  Architecture: "  << absInfo.arch() << std::endl
                   << "  Logical CPUs: "  << absInfo.logical_cpus() << std::endl
                   << "  Physical CPUs: " << absInfo.physical_cpus() << std::endl
                   << "  Memory: "        << absInfo.mem_total_bytes() << std::endl
                   << "  Page Size: "     << absInfo.page_size_bytes() << std::endl
                   << "  OS Version: "    << absInfo.os_version() << std::endl
                   << "  OS Build: "      << absInfo.os_build() << std::endl
                   << "  Kernel: "        << absInfo.kernel_release() << std::endl
                   << "  Model ID: "      << absInfo.model_id() << std::endl
                   << "  Hostname: "      << absInfo.hostname();
                return os;
            }
    };
}