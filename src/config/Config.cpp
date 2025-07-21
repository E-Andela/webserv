#include "Config.hpp"

Config::Config() = default;
Config::~Config() = default;

std::vector<ServerConfig>& Config::getServers() {
    return servers_;
}