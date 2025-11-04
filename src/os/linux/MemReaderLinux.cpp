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
#include "monitor/ansi.hpp"

namespace monitor::os::linux {
    class MemReader final : public monitor::os::AbstractMemReader {
        public:
            [[maybe_unused]] bool sample() override{
                return true;
            }

            void print(std::ostream& os) const override {
                os << monitor::ansi::BOLD << monitor::ansi::BLUE << "MemReaderLinux" << monitor::ansi::RESET << std::endl;
            }
    };
}

namespace monitor::os {
    std::unique_ptr<AbstractMemReader> make_mem_reader(){
        return std::make_unique<linux::MemReader>();
    }
}