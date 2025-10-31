#pragma once

#include <string>
#include <vector>
#include <map>

struct RouteConfig {
    std::string path;
    std::vector<std::string> methods;
    std::string uploadPath;
    std::string cgiPath;
    std::string cgiExtension;
    std::string redirectTo;
	std::string maxBodySize;
};

struct ServerConfig {
    int port = 0;
    std::string serverName;
    std::string root;
    std::string index;
    std::string bodyLimit;
    std::map<int, std::string> errorPages;
    std::vector<RouteConfig> routes;
};
