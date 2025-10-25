#pragma once
#include <vector>

namespace monitor::os {
    class AbstractPlatformInfo {
        public:
            virtual ~AbstractPlatformInfo()               = default;

            // CPU
            virtual std::string cpu_name()        const = 0; // e.g., "Apple M3 Pro", "Ryzen 7 5800X"
            virtual std::string arch()            const = 0; // e.g., "arm64", "x86_64"
            virtual uint32_t    logical_cpus()    const = 0;
            virtual uint32_t    physical_cpus()   const = 0;

            // Memory
            virtual uint64_t    mem_total_bytes() const = 0;
            virtual uint64_t    page_size_bytes() const = 0;

            // OS / machine
            virtual std::string os_version()      const = 0; // e.g., "macOS 16.0.1"
            virtual std::string os_build()        const = 0; // e.g., "25A362"
            virtual std::string kernel_release()  const = 0; // e.g., "25.0.0"
            virtual std::string model_id()        const = 0; // e.g., "MacBookPro18,3"
            virtual std::string hostname()        const = 0;            
    };
}