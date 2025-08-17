#include "Config/Logger.hpp"
#include "Config/ConfigParser.hpp"
#include "Config/ConfigValidator.hpp"
#include "Config/DebugPrint.hpp"
#include "Config/ConfigMapper.hpp"
#include "WebServer/WebServer.hpp"

void runConfigLogic(const std::string& configPath) {

    Logger::log(LOG_INFO, "Parsing config: " + configPath);

    // Parse config file into AST
    Config config = ConfigParser::parse(configPath);
    ConfigValidator::validate(config); 

    // Print raw config back
    // for (const Block& block : config.blocks) {
    //     // printBlock(block);
    // }

    // Print AST
    // printASTTree(config);
    
    // Map to server config
    std::vector<ServerConfig> servers = ConfigMapper::map(config);
	WebServer server(servers);
	server.initSockets();
	server.initPollManager();
	server.run();
    // printMappedConfig(servers);
}
