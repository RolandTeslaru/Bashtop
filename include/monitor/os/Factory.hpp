#pragma once
#include <memory>

namespace monitor::os {

    class AbstractCpuReader;
    class AbstractMemReader;
    class AbstractPlatformInfo;

    std::unique_ptr<AbstractCpuReader>      make_cpu_reader();
    std::unique_ptr<AbstractMemReader>      make_mem_reader();
    std::unique_ptr<AbstractPlatformInfo>   make_platform_info();
}