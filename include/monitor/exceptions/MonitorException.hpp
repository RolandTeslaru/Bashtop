#pragma once
#include <stdexcept>
#include <string>

namespace monitor::exceptions {

    class MonitorException : public std::runtime_error {
    public:
        explicit MonitorException(const std::string &message)
            : std::runtime_error("MonitorException: " + message) {}
    };  
}