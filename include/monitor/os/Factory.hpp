#pragma once
#include <memory>

// These factory function create the low level os readers at runtime

namespace monitor::os {
    class AbstractCpuReader;
    class AbstractPlatformInfoReader;
    class AbstractMemReader;

    std::unique_ptr<AbstractCpuReader>    make_cpu_reader();
    std::unique_ptr<AbstractPlatformInfoReader> make_platform_info_reader();
    std::unique_ptr<AbstractMemReader>    make_mem_reader();
}