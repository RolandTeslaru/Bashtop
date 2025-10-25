#pragma once
#include "monitor/types/Cpu.hpp"

using CpuRawSample = monitor::types::cpu::RawSample;

namespace monitor::os {
    using namespace monitor::types;
    
    // Even though CpuReader is explicit enough 
    // I like the verbosity of the abstract prefix
    class AbstractCpuReader {
        public:
            virtual ~AbstractCpuReader()              = default;
            virtual bool sample(CpuRawSample& out)    = 0;       // returns false on failure
    };

}