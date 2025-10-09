#include "Config/ConfigParser.hpp"
#include "Config/Logger.hpp"
#include "Config/DebugPrint.hpp"
#include "Config/ConfigValidator.hpp"
#include "Config/ConfigMapper.hpp"
#include "Config/ServerConfig.hpp"
#include "Config/ConfigError.hpp"
#include "Config/ConfigWrapper.hpp"
#include "WebServer/ParseHTTP.hpp"
#include <iostream>


int main(int argc, char* argv[])
{
    Logger::log(LOG_INFO, "Webserver starting . . .");

    std::string configPath;
    if (argc > 1)
        configPath = argv[1];
    else
        configPath = "config/default.conf";

    try {
        runConfigLogic(configPath);
    }

    catch (const ConfigError& ce) {
        Logger::log(LOG_ERROR, std::string("Config Error: ") + ce.what());
        return 1;
    }
    catch (const std::exception& e) {
        Logger::log(LOG_ERROR, std::string("Error: ") + e.what());
        return 1;
    }
    return 0;
}
