#pragma once
#include <stdexcept>
#include <string>

namespace monitor::exceptions {
    class EngineThreadException : public std::runtime_error {
    public:
        explicit EngineThreadException(const std::string &message)
            : std::runtime_error("EngineThreadException: " + message) {}
    };
};