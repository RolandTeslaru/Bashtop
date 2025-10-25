#pragma once

namespace monitor::os {

    class AbstractCpuReader {
        public:
            virtual ~AbstractCpuReader() = default;
            virtual bool sample() = 0;
            virtual int read() = 0;
    };

}