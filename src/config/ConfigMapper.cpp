#include "Config/ConfigMapper.hpp"
#include <stdexcept>
#include <sstream>

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

static std::map<int, std::string> getErrorPages(const Block& block) {
    std::map<int, std::string> errors;
    for (const Directive& d : block.directives) {
        if (d.name == "error_page" && d.args.size() == 2) {
            int code = std::stoi(d.args[0]);
            errors[code] = d.args[1];
        }
    }
    return errors;
}

static RouteConfig mapRoute(const Block& block) {
    RouteConfig route;
    if (!block.args.empty())
        route.path = block.args[0];

    route.methods = getDirectiveArgs(block, "methods");
    route.uploadPath = getDirectiveArg(block, "upload_path");
    route.cgiPath = getDirectiveArg(block, "cgi_path");
    route.cgiExtension = getDirectiveArg(block, "cgi_extension");
    route.redirectTo = getDirectiveArgs(block, "return").size() == 2
                       ? getDirectiveArgs(block, "return")[1] : "";

	route.maxBodySize = getDirectiveArg(block, "client_max_body_size");
    return route;
}

static ServerConfig mapServer(const Block& block) {
    ServerConfig server;
    server.port = std::stoi(getDirectiveArg(block, "listen"));
    server.serverName = getDirectiveArg(block, "server_name");
    server.root = getDirectiveArg(block, "root");
    server.index = getDirectiveArg(block, "index");
    server.bodyLimit = getDirectiveArg(block, "client_max_body_size");
    server.errorPages = getErrorPages(block);

    for (const Block& child : block.children) {
        if (child.name == "location") {
            server.routes.push_back(mapRoute(child));
        }
    }

    return server;
}

std::vector<ServerConfig> ConfigMapper::map(const Config& config) {
    std::vector<ServerConfig> servers;

    for (const Block& block : config.blocks) {
        if (block.name == "server") {
            servers.push_back(mapServer(block));
        }
    }

    return servers;
}
