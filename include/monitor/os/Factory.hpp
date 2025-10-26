#pragma once
#include <memory>

namespace monitor::os {
    class AbstractCpuReader;

    std::unique_ptr<AbstractCpuReader> make_cpu_reader();
}