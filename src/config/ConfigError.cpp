#include "ConfigError.hpp"

ConfigError::ConfigError(const std::string& msg)
    : std::runtime_error("ConfigError: " + msg) {}

ConfigParseError::ConfigParseError(const std::string& msg)
    : ConfigError("ParseError: " + msg) {}

ConfigValidationError::ConfigValidationError(const std::string& msg)
    : ConfigError("ValidationError: " + msg) {}

ConfigMappingError::ConfigMappingError(const std::string& msg)
    : ConfigError("MappingError: " + msg) {}

/*
fatal:
    Validator
    - missing listen
    - invalid port
    - location without path
    - invalid HTTP method in methods
    - faulty error_pages : error_page <status_code> <path>;
    - faulty return : return <status_code> <url>;
                    return <status_code>;

    Mapper:
    - missing listen/ bad conversion
    - bad error_page codde conversion
    - any conversion failure

Warnings:
    Validator
    - missing server_name
    - bad error_page code conversion 
    - bad conversions 
    (throw error instead of having stoi crash - by configMapper)
*/
