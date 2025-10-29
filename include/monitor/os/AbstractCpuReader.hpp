#include "monitor/types/Cpu.hpp"
#include <chrono>

namespace monitor::os {    
    // Contract like class for CPU readers

    class AbstractCpuReader {
        public:
            virtual ~AbstractCpuReader() = default;
            
            // Reads ticks from the os.
            virtual bool sample(
                monitor::types::cpu::RawSample& out
            ) = 0;      

            static uint64_t toNanoseconds(const auto &duration){
                return static_cast<uint64_t>(
                    std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()
                );
            }
    };

}