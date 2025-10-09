#include "Config/DebugPrint.hpp"
#include <iostream>
#include <string>


void printBlock(const Block& block, int indent) {
    std::string pad(indent, ' ');
    std::cout << pad << block.name;
    for (const std::string& arg : block.args) std::cout << " " << arg;
    std::cout << " {\n";

    for (const Directive& d : block.directives) {
        std::cout << pad << "  " << d.name;
        for (const std::string& arg : d.args) std::cout << " " << arg;
        std::cout << ";\n";
    }

    for (const Block& child : block.children) {
        printBlock(child, indent + 2);
    }

    std::cout << pad << "}\n";
}


static void printIndent(int indent) {
    std::cout << std::string(indent * 2, ' ');
}

static void printDirective(const Directive& d, int indent) {
    printIndent(indent);
    std::cout << "📘 Directive: " << d.name;
    if (!d.args.empty()) {
        std::cout << " [args:";
        for (const std::string& arg : d.args)
            std::cout << " \"" << arg << "\"";
        std::cout << " ]";
    }
    std::cout << "\n";
}

static void printBlockTree(const Block& block, int indent = 0) {
    printIndent(indent);
    std::cout << "📦 Block: " << block.name;
    if (!block.args.empty()) {
        std::cout << " [args:";
        for (const std::string& arg : block.args)
            std::cout << " \"" << arg << "\"";
        std::cout << " ]";
    }
    std::cout << "\n";

    for (const Directive& d : block.directives) {
        printDirective(d, indent + 1);
    }

    for (const Block& child : block.children) {
        printBlockTree(child, indent + 1);
    }
}

void printASTTree(const Config& config) {
    std::cout << "\n🔍 AST Tree Structure:\n";
    for (const Block& block : config.blocks) {
        printBlockTree(block);
    }
    std::cout << std::endl;
}



void printMappedConfig(const std::vector<ServerConfig>& servers) {
    for (const ServerConfig& server : servers) {
        std::cout << "\nMapped ServerConfig:\n";
        std::cout << "  Port: " << server.port << "\n";
        std::cout << "  Server Name: " << server.serverName << "\n";
        std::cout << "  Root: " << server.root << "\n";
        std::cout << "  Index: " << server.index << "\n";
        std::cout << "  Body Limit: " << server.bodyLimit << "\n";
        for (const auto& ep : server.errorPages)
            std::cout << "  Error " << ep.first << ": " << ep.second << "\n";

        for (const RouteConfig& route : server.routes) {
            std::cout << "  Route: " << route.path << "\n";
            if (!route.methods.empty()) {
                std::cout << "    Methods: ";
                for (const std::string& m : route.methods)
                    std::cout << m << " ";
                std::cout << "\n";
            }
            if (!route.uploadPath.empty())
                std::cout << "    Upload Path: " << route.uploadPath << "\n";
            if (!route.cgiPath.empty())
                std::cout << "    CGI Path: " << route.cgiPath << "\n";
            if (!route.cgiExtension.empty())
                std::cout << "    CGI Ext: " << route.cgiExtension << "\n";
            if (!route.redirectTo.empty())
                std::cout << "    Redirect To: " << route.redirectTo << "\n";
        }
    }
}
