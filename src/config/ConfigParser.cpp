#include "ConfigParser.hpp"
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>



/* reads config file line by line
recognizes server blocks
parses directives
calls/builds other parsers(routeconfig - serverconfig */

//TODO: whitespace trimmer function
// Questions: can we use namespace detail {...} for helper functions?

static std::string whitespaceTrimmer(std::string_view sv)
{
    const char *whitespace = " \t\r\n";
    std::size_t start = sv.find_first_not_of(whitespace);
    std::size_t end = sv.find_last_not_of(whitespace);

    if (start == std::string_view::npos)
        return {}; //pass or skip keyword ?
    std::size_t len = end - start + 1;
    std::string_view middle = sv.substr(start, len);
    return std::string{middle};
}

// TODO: commentStripper() - strips everything after #
// Question: use 'auto' instead of std::size_t ?
static std::string_view commentStripper(std::string_view sv) {
    std::size_t pos = sv.find('#');
    if (pos == std::string_view::npos)
        return sv;
    else
        return sv.substr(0, pos);
}

// Question: is there an internal cpp thing to use instead?
static inline bool isSpace(char c) {
    return (c == ' ' || c == '\t');
}

// TODO: tokenizer function - splits on space or tab
std::vector<std::string> tokenizer(std::string_view sv) {
    std::vector<std::string> tokens;
    std::size_t i = 0;
    while (i < sv.size()) {
        while (i < sv.size() && isSpace(sv[i]))
            i++;
        if (i >= sv.size()) // line was entirely blank
            break;
        std::size_t start = i;
        while (i < sv.size() && !isSpace(sv[i]))
            i++;
        tokens.emplace_back(sv.substr(start, i - start)); // emplace_back similar to push_back - slice and copy into the vector
    }
    return tokens;
}


static void serverDirectives(ServerConfig& srv,
                             const std::vector<std::string>& tok)
{
    if (tok.empty()) return;

    // listen 8080;
    if (tok[0] == "listen" && tok.size() >= 2) {
        std::string portStr = tok[1];
        if (!portStr.empty() && portStr.back() == ';')
            portStr.pop_back();                 // strip ';'
        srv.port = std::stoi(portStr);
    }

    // Anything else is ignored for this minimal test
}

static void locationDirectives(RouteConfig& loc,
                               const std::vector<std::string>& tok)
{
    if (tok.empty()) return;

    // allow_methods GET POST;
    if (tok[0] == "allow_methods" && tok.size() >= 2) {
        loc.methods.clear();
        for (std::size_t i = 1; i < tok.size(); ++i) {
            std::string m = tok[i];
            if (!m.empty() && m.back() == ';')
                m.pop_back();                   // strip ';'
            loc.methods.push_back(m);
        }
    }

    // Anything else is ignored for this minimal test
}


//TODO: parseServerBlock() function
//TODO: parseLocationBlock() function
//TODO: parse() function

// TODO: open file and read lines
static std::vector<std::string> readFileLines(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + path);
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}


Config ConfigParser::parse(const std::string& path) {
    const auto lines = readFileLines(path);
    // error check lines

    // var
    Config config;
    ServerConfig currentServer;
    RouteConfig currentRoute;
    bool inServer = false;
    bool inLocation = false;

    for (std::size_t i = 0; i < lines.size(); i++) {
        std::string cleanedLines = whitespaceTrimmer(commentStripper(lines[i]));
        if (cleanedLines.empty())
            continue;
        auto token = tokenizer(cleanedLines);

        if (token.size() == 2 && token[0] == "server" && token[1] == "{") {
            currentServer = ServerConfig{};
            inServer = true;
            continue;
            
        }

        if (token.size() == 3 && token[0] == "location" && token[1] == "{") {
            currentRoute = RouteConfig{};
            currentRoute.path = token[1];
            inLocation = true;
            continue;
        }

        // closing braces
        if (token.size() == 1 && token[0] == "}") {
            if (inServer) {
                config.getServers().push_back(currentServer);
                inServer = false;
                continue;

            }
            if (inLocation) {
                currentServer.routes.push_back(currentRoute);
                inLocation = false;
                continue;
            }
            // error
        }

        // handle directives
        if (inLocation)
            locationDirectives(currentRoute, token);
        else if (inServer)
            serverDirectives(currentServer, token);
        //else
            // error
        

        // EOF and checks
    }
    return config;
}



/*
open file at path
store each line in a vector string
readFileLines()

for each line {
    trim whitespce
    strip comments
    skip blank lines
    tokenize into words

    if server { ===> new ServerConfig - move i to } - next line
    if location /path ===> new RouteConfig  - move i to } - next line
    if } ===> close that block
}
*/



