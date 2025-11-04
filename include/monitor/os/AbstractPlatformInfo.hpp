#pragma once
#include <vector>
#include <ostream>
#include <string>
#include <cstdint>

#include <iomanip> // for cool column formatting

#include "monitor/ansi.hpp"

namespace monitor::os
{
    class AbstractPlatformInfo
    {
    protected:
        std::string cpu_name       = "unknown";
        std::string arch           = "unknown";
        uint32_t logical_cpus      = 0;
        uint32_t physical_cpus     = 0;
        uint64_t mem_total_bytes   = 0;
        uint64_t page_size_bytes   = 0;
        std::string os_version     = "unknown";
        std::string os_build       = "unknown";
        std::string kernel_release = "unknown";
        std::string model_id       = "unknown";
        std::string hostname       = "unknown";

    public:
        AbstractPlatformInfo() = default;
        virtual ~AbstractPlatformInfo() = default;

        // CPU
        virtual std::string getCpuName()       const { return cpu_name; }
        virtual std::string getArch()          const { return arch; }
        virtual uint32_t    getLogicalCpus()   const { return logical_cpus; }
        virtual uint32_t    getPhysicalCpus()  const { return physical_cpus; }
        virtual uint64_t    getMemTotalBytes() const { return mem_total_bytes; }
        virtual uint64_t    getPageSizeBytes() const { return page_size_bytes; }
        virtual std::string getOsVersion()     const { return os_version; }
        virtual std::string getOsBuild()       const { return os_build; }
        virtual std::string getKernelRelease() const { return kernel_release; }
        virtual std::string getModelId()       const { return model_id; }
        virtual std::string getHostname()      const { return hostname; }

        AbstractPlatformInfo(const AbstractPlatformInfo &absInfo)
        {
            cpu_name        = absInfo.cpu_name;
            arch            = absInfo.arch;
            logical_cpus    = absInfo.logical_cpus;
            physical_cpus   = absInfo.physical_cpus;
            mem_total_bytes = absInfo.mem_total_bytes;
            page_size_bytes = absInfo.page_size_bytes;
            os_version      = absInfo.os_version;
            os_build        = absInfo.os_build;
            kernel_release  = absInfo.kernel_release;
            model_id        = absInfo.model_id;
            hostname        = absInfo.hostname;
        }

        AbstractPlatformInfo &operator=(const AbstractPlatformInfo &absInfo)
        {
            if (this == &absInfo)
                return *this;
            cpu_name        = absInfo.cpu_name;
            arch            = absInfo.arch;
            logical_cpus    = absInfo.logical_cpus;
            physical_cpus   = absInfo.physical_cpus;
            mem_total_bytes = absInfo.mem_total_bytes;
            page_size_bytes = absInfo.page_size_bytes;
            os_version      = absInfo.os_version;
            os_build        = absInfo.os_build;
            kernel_release  = absInfo.kernel_release;
            model_id        = absInfo.model_id;
            hostname        = absInfo.hostname;
            return *this;
        }

        friend std::ostream &operator<<(
            std::ostream &os, const AbstractPlatformInfo &absInfo
        ){
            const int labelWidth = 18; // width for the label column
            
            os << monitor::ansi::BOLD << monitor::ansi::GREEN << "Platform Info: " << monitor::ansi::RESET << std::endl
               << "  " << std::left << std::setw(labelWidth) << "CPU:" 
               << absInfo.getCpuName() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "Architecture:" 
               << absInfo.getArch() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "Logical CPUs:" 
               << absInfo.getLogicalCpus() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "Physical CPUs:" 
               << absInfo.getPhysicalCpus() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "Memory (bytes):" 
               << absInfo.getMemTotalBytes() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "Page Size:" 
               << absInfo.getPageSizeBytes() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "OS Version:" 
               << absInfo.getOsVersion() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "OS Build:" 
               << absInfo.getOsBuild() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "Kernel:" 
               << absInfo.getKernelRelease() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "Model ID:" 
               << absInfo.getModelId() << std::endl
               
               << "  " << std::left << std::setw(labelWidth) << "Hostname:" 
               << absInfo.getHostname() << std::endl;
            
            return os;
        }
    };
}