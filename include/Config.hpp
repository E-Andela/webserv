#pragma once
#include <string>
#include <vector>
#include <map>

// Location block
struct RouteConfig {
    std::string path;
    std::vector<std::string> methods;
    std::string index;
    //std::string uploadPath;
    //std::string cgiPath;
    //std::string cgiExtension;
    std::string redirect;
    bool autoindex = false;
};

// Server block
struct ServerConfig {
    std::string host = "0.0.0.0"; // init here or in .cpp?
    int port = 0;
    std::string root;
    std::string serverName;
    size_t maxBodySize; // init
    std::map<int, std::string> errorPages;
    std::vector<RouteConfig> routes; 
};

class Config {
private:
    std::vector<ServerConfig> servers_;
    friend class ConfigParser; // is friend keyword allowed? 

public:
    Config();
    // Config(const Config& copy);
    // Config& operator=(const Config& copy);
    ~Config();
    // Config(Config&& copy) noexcept; // Move constructor
    // Config& operator=(Config&& copy) noexcept; // Move ass operator

    std::vector<ServerConfig>& getServers();
};
