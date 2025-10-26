#pragma once
#include "monitor/types/Cpu.hpp"

namespace monitor::os {    
    // Contract like class for CPU readers
    // Derived classes MUST implement the sample method
    class AbstractCpuReader {
        public:
            virtual ~AbstractCpuReader() = default;
            // reads the current CPU usage sample in out
            // returns false on failure
            virtual bool sample(
                monitor::types::cpu::RawSample& out
            ) = 0;      
    };

}