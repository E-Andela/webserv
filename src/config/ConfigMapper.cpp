#include "ConfigMapper.hpp"
#include "ConfigError.hpp"
#include <sstream>
#include <map>

static std::string getDirectiveArg(const Block& block, const std::string& name) {
    for (const Directive& d : block.directives) {
        if (d.name == name && !d.args.empty())
            return d.args[0];
    }
    return "";
}

static std::vector<std::string> getDirectiveArgs(const Block& block, const std::string& name) {
    for (const Directive& d : block.directives) {
        if (d.name == name)
            return d.args;
    }
    return {};
}

static int safeStoi(const std::string& s, const std::string& fieldName) {
    try {
        return std::stoi(s);
    } catch (...) {
        throw ConfigMappingError("Invalid numeric value for " + fieldName + ": '" + s + "'.");
    }
}

static std::map<int, std::string> getErrorPages(const Block& block) {
    std::map<int, std::string> errors;
    for (const Directive& d : block.directives) {
        if (d.name == "error_page" && d.args.size() == 2) {
            const std::string& codeString = d.args[0];
            const std::string& path = d.args[1];
            int code = safeStoi(codeString, "error_page");
            errors[code] = path;
        }
    }
    return errors;
}

static std::string getReturnTarget(const Block& block) {
    for (const Directive& d : block.directives) {
        if (d.name == "return") {
            if (d.args.empty())
                return "";
            (void)safeStoi(d.args[0], "return");
            if (d.args.size() >= 2)
                return d.args[1];
        }
    }
    return "";
}

static RouteConfig mapRoute(const Block& block) {
    RouteConfig route;
    if (!block.args.empty())
        route.path = block.args[0];

    route.methods       = getDirectiveArgs(block, "methods");
    route.uploadPath    = getDirectiveArg(block, "upload_path");
    route.cgiPath       = getDirectiveArg(block, "cgi_path");
    route.cgiExtension  = getDirectiveArg(block, "cgi_extension");
    route.redirectTo    = getReturnTarget(block);
    // route.redirectTo    = getDirectiveArgs(block, "return").size() == 2
    //                    ? getDirectiveArgs(block, "return")[1] : "";
    return route;
}

// define mandatory fields
static ServerConfig mapServer(const Block& block) {
    ServerConfig server;

    {
        std::string portString = getDirectiveArg(block, "listen");
        if (portString.empty())
            throw ConfigMappingError("Server block missing 'listen' directive");
        server.port = safeStoi(portString, "listen");
    }

    server.port = std::stoi(getDirectiveArg(block, "listen"));
    server.serverName = getDirectiveArg(block, "server_name");
    server.root = getDirectiveArg(block, "root");
    server.index = getDirectiveArg(block, "index");
    server.bodyLimit = getDirectiveArg(block, "client_max_body_size");
    server.errorPages = getErrorPages(block);

    for (const Block& child : block.children) {
        if (child.name == "location")
            server.routes.push_back(mapRoute(child));
    }
    return server;
}

std::vector<ServerConfig> ConfigMapper::map(const Config& config) {
    std::vector<ServerConfig> servers;
    for (const Block& block : config.blocks) {
        if (block.name == "server")
            servers.push_back(mapServer(block));
    }
    return servers;
}
