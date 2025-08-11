#pragma once

#include "AST.hpp"
#include "ServerConfig.hpp"
#include <vector>

class ConfigMapper {
public:
    static std::vector<ServerConfig> map(const Config& config);
};
