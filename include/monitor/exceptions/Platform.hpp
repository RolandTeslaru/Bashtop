#pragma once
#include <stdexcept>
#include <string>

namespace monitor::exceptions {
    class PlatformNotSupportedException: public std::runtime_error {
    public: 
        explicit PlatformNotSupportedException(const std::string& message)
            : std::runtime_error("PlatformNotSupportedException: " + message) {}
    };
}