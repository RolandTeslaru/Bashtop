#include "monitor/types/Mem.hpp"
#include <chrono>

namespace monitor::os {    
    // Contract like class for CPU readers

    class AbstractMemReader {
        public:
            virtual ~AbstractMemReader() = default;
            
            virtual bool sample() = 0;      

    };

}