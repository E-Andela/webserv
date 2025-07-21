#include "ConfigParser.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include <iostream>
#include <cstring>
#include <signal.h>


//int main(int argc, char *argv[]) {

    // std::cout << "Webserver . . ." << std::endl;

    // // read config file path
    // std::string configPath;
    // if (argc > 1) {
    //     configPath = argv[1];
    // } else {
    //     configPath = "config/default.conf";
    // }

    // // TODO: read and parse config file
    // Config config = ConfigParser::parse(configPath);

    // // TODO: validate parsed config
    // //ConfigValidator::validate(config);

    // // std::cout << "Config loaded with lines:" << std::endl;
    // // for (const std::string& line : config.getLines()) {
    // //     std::cout << "> " << line << std::endl;
    // // }

    // // TODO: initialze servers from config

    // // TODO: launch poll() based event loop to handle connections


    // return 0;
//}




static void dumpConfig(Config& cfg)
{
    std::cout << "Servers found: " << cfg.getServers().size() << "\n";

    for (const ServerConfig& srv : cfg.getServers()) {
        std::cout << "--------------------------------------------------\n";
        std::cout << "Port        : " << srv.port       << "\n";
        std::cout << "Root        : " << srv.root       << "\n";
        std::cout << "Server name : " << srv.serverName << "\n";
        std::cout << "Routes      : " << srv.routes.size() << "\n";

        for (const RouteConfig& loc : srv.routes) {
            std::cout << "  • location " << loc.path << "\n";
            std::cout << "      methods:";
            for (const std::string& m : loc.methods) std::cout << ' ' << m;
            if (loc.methods.empty()) std::cout << " (none)";
            std::cout << "\n";
        }
    }
    std::cout << "--------------------------------------------------\n";
}

int main(int argc, char* argv[])
{
    std::cout << "Webserver . . ." << std::endl;

    // read config file path
    std::string configPath;
    if (argc > 1) {
        configPath = argv[1];
    } else {
        configPath = "config/default.conf";
    }

    try {
        Config cfg = ConfigParser::parse(configPath);
        dumpConfig(cfg);
    }
    catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
