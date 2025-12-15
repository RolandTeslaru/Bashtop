#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <cctype>
#include <iostream>

#include <mach/mach.h>
#include <mach/mach_host.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#include "monitor/os/AbstractMemReader.hpp"
#include "monitor/ansi.hpp"
#include "monitor/exceptions/SampleExceptions.hpp"

namespace monitor::os::mac {
    class MemReader final : public monitor::os::AbstractMemReader {
        public:
            void sample(monitor::types::mem::RawSample& out) override {
                // Get total memory
                int mib[2];
                mib[0] = CTL_HW;
                mib[1] = HW_MEMSIZE;
                int64_t physical_memory = 0;
                size_t length = sizeof(int64_t);
                sysctl(mib, 2, &physical_memory, &length, NULL, 0);
                out.total = physical_memory;

                // Get VM stats
                mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
                vm_statistics64_data_t vm_stats;
                if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vm_stats, &count) != KERN_SUCCESS) 
                    throw monitor::exceptions::MemSampleException("Failed to get VM stats from host");

                vm_size_t page_size;
                host_page_size(mach_host_self(), &page_size);

                out.free = (uint64_t)vm_stats.free_count * page_size;
                
                // used mem = active + inactive + wired
                out.used = ((uint64_t)vm_stats.active_count + 
                            (uint64_t)vm_stats.inactive_count + 
                            (uint64_t)vm_stats.wire_count) * page_size;

                // Get Swap stats
                struct xsw_usage vmusage{};
                size_t size = sizeof(vmusage);
                if (sysctlbyname("vm.swapusage", &vmusage, &size, NULL, 0) == 0) {
                    out.swapTotal = vmusage.xsu_total;
                    out.swapUsed  = vmusage.xsu_used;
                    out.swapFree  = vmusage.xsu_avail;
                } else {
                    out.swapTotal = 0;
                    out.swapUsed = 0;
                    out.swapFree = 0;
                }
            }

            void print(std::ostream& os) const override {
                os << monitor::ansi::BOLD << monitor::ansi::BLUE << "MemReaderMac" << monitor::ansi::RESET << std::endl;
            }
    };
}

namespace monitor::os {
    std::unique_ptr<AbstractMemReader> make_mem_reader(){
        return std::make_unique<mac::MemReader>();
    }
}