#include <cstdint>
#include <string>

#include "monitor/os/AbstractPlatformInfo.hpp"

namespace monitor::os::mac {
    class PlatformInfoMac final : public monitor::os::AbstractPlatformInfo {
        private:
            std::string cpu_name_;
            std::string arch_;
            uint32_t    logical_cpus_    = 0;
            uint32_t    physical_cpus_   = 0;
            uint64_t    mem_total_bytes_ = 0;
            uint64_t    page_size_bytes_ = 0;
            std::string os_version_;
            std::string os_build_;
            std::string kernel_release_;
            std::string model_id_;
            std::string hostname_;

        public:
            ~PlatformInfoMac() override = default;
    };
}