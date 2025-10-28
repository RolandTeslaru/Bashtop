#pragma once
#include <memory>

namespace monitor::os {
    class AbstractCpuReader;
    class AbstractPlatformInfo;

    std::unique_ptr<AbstractCpuReader> make_cpu_reader();
    std::unique_ptr<AbstractPlatformInfo> make_platform_info();
}