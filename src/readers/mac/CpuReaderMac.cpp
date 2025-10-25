// To read the CPU usage on Mac systems,
// we can use the Mach host API to get CPU load information.


#include "monitor/os/AbstractCpuReader.hpp"



namespace monitor::os::mac {
    class CpuReader final : public monitor::os::AbstractCpuReader {
        public:
            bool sample() override {
                return true;
            }
    };
};