
#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <cctype>
#include <iostream>

#include "monitor/os/AbstractMemReader.hpp"

namespace monitor::os::mac {
    class MemReader final : public monitor::os::AbstractMemReader {
        public:
            MemReader(){

            }

            bool sample() override {
                return true;
            }
    };
}

namespace monitor::os {
    std::unique_ptr<AbstractMemReader> make_mem_reader(){
        return std::make_unique<mac::MemReader>();
    }
}