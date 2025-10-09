#include "Config/ConfigError.hpp"

ConfigError::ConfigError(const std::string& msg)
    : std::runtime_error("ConfigError: " + msg) {}

ConfigParseError::ConfigParseError(const std::string& msg)
    : ConfigError("ParseError: " + msg) {}

ConfigValidationError::ConfigValidationError(const std::string& msg)
    : ConfigError("ValidationError: " + msg) {}

ConfigMappingError::ConfigMappingError(const std::string& msg)
    : ConfigError("MappingError: " + msg) {}
