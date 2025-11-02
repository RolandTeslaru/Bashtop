#pragma once
#include <chrono>

namespace monitor::os {    

    class AbstractMemReader {
        public:
            virtual ~AbstractMemReader() = default;
            
            virtual bool sample() = 0;      

    };

}