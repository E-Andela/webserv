#pragma once

#include <stdexcept>
#include <string>

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg);
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& msg);
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& msg);
};

class ConfigMappingError : public ConfigError {
public:
    explicit ConfigMappingError(const std::string& msg);
};
