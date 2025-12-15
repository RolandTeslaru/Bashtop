#pragma once
#include <stdexcept>
#include <string>

namespace monitor::exceptions {

    class SampleException : public std::runtime_error {
    public: 
        explicit SampleException(const std::string &message)
            : std::runtime_error("SampleException: " + message) {}
    };

    class CpuSampleException : public SampleException {
    public:
        explicit CpuSampleException(const std::string &message)
            : SampleException("CPU sampling failed: " + message) {}
    };

    class MemSampleException : public SampleException {
    public:
        explicit MemSampleException(const std::string &message)
            : SampleException("Memory sampling failed: " + message) {}
    };

    class ValueException: public std::runtime_error {
    public:
        explicit ValueException(const std::string &message)
            : std::runtime_error("ValueException: " + message) {}
    }

}