#pragma once
#include "Config.hpp"
#include <string>

class ConfigParser {

public:
    static Config parse(const std::string& path);
};
