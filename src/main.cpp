#include "ConfigParser.hpp"
#include "Logger.hpp"
#include "DebugPrint.hpp"
#include "ConfigValidator.hpp"
#include "ConfigMapper.hpp"
#include "ServerConfig.hpp"
#include "ConfigError.hpp"
#include "ConfigWrapper.hpp"
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


/*
TODO:
Use ConfigError in different places of the code
decide what is warning what is error
remove all std::cerr 
refactor configparser and configmapper
make sure token STRING is being used where it needs to be used
for fatal validation errors use configerror: (swwitcg real errors to exceptions) - warnings as logs
    missing mandatory directives
    invalid ports
    invalid http methods
    problems with error_pages


Tokenizer → Parser → Validator → Mapper → Wrapper → main
(throw) (throw) (throw) (throw) (catch/log) (catch/exit)


*/
