#pragma once
#include "monitor/types/Cpu.hpp"
#include <chrono>

namespace monitor::os {    
    // Contract like class for CPU readers

    class AbstractCpuReader {
        public:
            // ====================================================================
            // Constructor / Destructor
            // ====================================================================
            AbstractCpuReader() = default;
            virtual ~AbstractCpuReader() = default;
            
            // ====================================================================
            // Copy constructyor
            // ====================================================================
            AbstractCpuReader(const AbstractCpuReader& other) = default;

            virtual AbstractCpuReader* clone() const = 0;

            // ====================================================================
            // Operator Overloads
            // ====================================================================
            AbstractCpuReader& operator=(const AbstractCpuReader& other) = default;

            // cannot make the operator<< pure virtual, so we use this print helper.
            virtual void print(std::ostream& os) const = 0;
            friend std::ostream& operator<<(std::ostream& os, const AbstractCpuReader& reader) { 
                reader.print(os);
                return os;
            }

            // Reads ticks from the os.
            virtual void sample(
                monitor::types::cpu::RawSample& out
            ) = 0;      

            static uint64_t toNanoseconds(const auto &duration){
                return static_cast<uint64_t>(
                    std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()
                );
            }
    };

}