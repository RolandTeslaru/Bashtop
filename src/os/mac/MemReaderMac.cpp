#ifdef __APPLE__

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

#include "monitor/os/mac/MemReaderMac.hpp"




namespace monitor::os::mac
{

    using MemRawSample       = monitor::types::mem::RawSample;
    using MemSampleException = monitor::exceptions::MemSampleException;


    void MemReader::sample(MemRawSample &out)
    {
        int sys_call_return;

        // fun fact: macOS organises system info in a tree like structure,

        // CTL_HW is the hardware branch
        // HW_MEMSIZE is the leaf representing physical memory size
        int mib[2] = { CTL_HW, HW_MEMSIZE }; // actual path: /CTL_HW/HW_MEMSIZE

        int64_t physical_memory = 0;
        size_t length = sizeof(int64_t);
        
        // sysctl call with the mib path to get physical memory size
        sys_call_return = sysctl(mib, 2, &physical_memory, &length, NULL, 0); 

        if (sys_call_return != 0)
            throw MemSampleException("Mach sys call failed. Could not get physical memory size via sysctl.");
        
        out.total = physical_memory;

        // get virtual memory stats
        mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
        vm_statistics64_data_t vm_stats;

        kern_return_t kernel_call_return;

        kernel_call_return = host_statistics64(
            mach_host_self(),
            HOST_VM_INFO64, // number representing the type of info to ask the kernel for
            (host_info64_t)&vm_stats, 
            &count
        );

        if(kernel_call_return != KERN_SUCCESS)
            throw MemSampleException("Mach statistics kernel call failed. Returned: " + std::to_string(kernel_call_return));

        vm_size_t page_size;
        kernel_call_return = host_page_size(mach_host_self(), &page_size);

        if (kernel_call_return != KERN_SUCCESS)
            throw MemSampleException("Mach page size kernel call failed.");

        out.free = (uint64_t)vm_stats.free_count * page_size;

        // used mem = active + inactive + wired
        out.used = (
            (uint64_t)vm_stats.active_count +
            (uint64_t)vm_stats.inactive_count +
            (uint64_t)vm_stats.wire_count
        ) *
            page_size;

        // Get Swap stats
        struct xsw_usage vmusage{};
        size_t size = sizeof(vmusage);
                                                // shortcut to get swap usage ( /vm/swapusage )
        sys_call_return = sysctlbyname("vm.swapusage", &vmusage, &size, NULL, 0);

        if (sys_call_return == 0)
        {
            out.swapTotal = vmusage.xsu_total;
            out.swapUsed = vmusage.xsu_used;
            out.swapFree = vmusage.xsu_avail;
        }
        else
        {
            out.swapTotal = 0;
            out.swapUsed = 0;
            out.swapFree = 0;
        }
    }

    void MemReader::print(std::ostream &os) const
    {
        os << monitor::ansi::BOLD << monitor::ansi::BLUE << "MemReaderMac" << monitor::ansi::RESET << std::endl;
    }

}

namespace monitor::os
{
    // Engine monitors usually take ownership of the readers
    std::unique_ptr<AbstractMemReader> make_mem_reader()
    {
        return std::make_unique<mac::MemReader>();
    }
}

#endif