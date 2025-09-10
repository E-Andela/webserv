#include "ConfigValidator.hpp"
#include "Logger.hpp"
#include "ConfigError.hpp"
#include <iostream>
#include <set>
#include <algorithm>
#include <cctype>
#include <string>

#define VALID_TCP_UDP_PORT 65535

static bool is_digit_string(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

static bool parse_port(const std::string& s, int& out) {
    if (!is_digit_string(s))
        return false;
    try {
        out = std::stoi(s);
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
    return out > 0 && out <= VALID_TCP_UDP_PORT;
}



/* server only directives */
static const std::set<std::string> serverDirectives = {
    "listen", "server_name", "root", "index",
    "error_page", "client_max_body_size"
};


/* valid HTTP methods */
static const std::set<std::string> validMethods = { "GET", "POST", "DELETE" };

/*
validate presense of mandatory fields
*/
static void validateServerDirective(const Directive& d, bool& hasListen, bool& hasServerName) {

    // checks directive's name
    if (serverDirectives.find(d.name) == serverDirectives.end()) {
        Logger::log(LOG_ERROR, "Unknown directive '" + d.name + "' in server block."); // error or warning?
        return;
    }

    if (d.name == "listen") {
        hasListen = true;
        if (d.args.empty())
            throw ConfigValidationError("'listen' directive missing argument.");
        if (d.args.size() != 1) {
        // in case of missing ;
        //std::string hint;
        //if (d.args.size() >= 2) {
            //hint = " (did you forget ';' before '" + d.args[1] + "'?)";
        //}
        throw ConfigValidationError(
            "'listen' expects exactly one argument; got " +
            std::to_string(d.args.size()) //+ hint
        );
    }
        int port;
        if (!parse_port(d.args[0], port))
            throw ConfigValidationError("Invalid port in 'listen'.");
    }
    
    else if (d.name == "server_name") {
        hasServerName = true;
        if (d.args.empty())
            Logger::log(LOG_WARNING, "'server_name' is empty.");

    } else if ((d.name == "root" || d.name == "index") && d.args.empty()) {
        //Logger::log(LOG_ERROR, "No value for: " + d.name +".");
        throw ConfigValidationError("'" + d.name + "'requires a value.");
    }

    else if (d.name == "error_page") {
        if (d.args.size() != 2 || !is_digit_string(d.args[0])) {
            //Logger::log(LOG_ERROR, "'error_page' must be: error_page <code> <path>;");
            throw ConfigValidationError("expected 'error_page <code> <path>'");
        }
    }
    // client_max_body_size
    // validate more 
}


/* valid Location directives */
static const std::set<std::string> locationDirectives = {
    "methods", "upload_path", "return",
    "cgi_path", "cgi_extension"
};

static void validateLocationDirective(const Directive& d, const std::string& locationPath) {
    if (locationDirectives.find(d.name) == locationDirectives.end()) {
        Logger::log(LOG_WARNING, "Unknown directive '" + d.name + "in location '" + locationPath + "'.");
        return ;
    }

    if (d.name == "upload_path" || d.name == "cgi_path" || d.name == "cgi_extension") {
        if (d.args.empty()) {
            //Logger::log(LOG_ERROR, "'" + d.name + "' requires a value.");
            throw ConfigValidationError("'" + d.name + "' requires a value in location '" + locationPath + "'.");
        }
    }

    if (d.name == "return") {
        if (d.args.empty()) {
            //Logger::log(LOG_ERROR, "'return' directive requires a status code in location '");
            throw ConfigValidationError("return no good in '" + locationPath + "'.");
        } if (!is_digit_string(d.args[0])) {
            //Logger::log(LOG_ERROR, "'return' code must be numeric.");
            throw ConfigValidationError("return code must be numeric in location '" + locationPath + "'.");
            // check non empty url if exists
        }
    }

    if (d.name == "methods") {
        if (d.args.empty()) {
            //Logger::log(LOG_ERROR, "'methods' must list at least one method.");
            throw ConfigValidationError("must at least list one method in '" + locationPath + "'.");
        }
        for (const std::string& m : d.args) {
            if (validMethods.find(m) == validMethods.end()) {
                //Logger::log(LOG_ERROR, "Invalid HTTP method '" + m + "'.");
                throw ConfigValidationError("invalid http method '" + m + "'in location '" + locationPath + "'.");
            }
        }
    }
}



static void validateLocationBlock(const Block& block) {
    if (block.args.empty()) {
        //Logger::log(LOG_ERROR, "location block missing path.");
        throw ConfigValidationError("location block missing path.");
        //return;
    }
    const std::string& locationPath = block.args[0];
    bool methodsSeen = false;

    for (const Directive& d : block.directives) {
        if (d.name == "methods") {
            if (methodsSeen)
                Logger::log(LOG_WARNING, "Duplicate 'methods' in location '" + locationPath + "'. the first one will be used.");
            methodsSeen = true;
        }
        validateLocationDirective(d, locationPath);
    }

    if (!methodsSeen) {
        Logger::log(LOG_WARNING,
            "location '" + locationPath + "' has no 'methods' directive.");
    }
       // Disallow nested blocks inside location (only directives expected) - warn
    for (const Block& child : block.children) {
        Logger::log(LOG_WARNING,
            "unknown nested block '" + child.name + "' inside location '" + locationPath + "'.");
    }
}

/*
Server block validation
*/
static void validateServerBlock(const Block& block) {
    bool hasListen = false;
    bool hasServerName = false;
    bool listenSeen = false;
    bool serverNameSeen = false;

    for (const Directive& d : block.directives) {
        if (d.name == "listen") {
            if (listenSeen)
                Logger::log(LOG_WARNING, "Duplicate 'listen' in server block. The first one will be used.");
            listenSeen = true;
        }
        if (d.name == "server_name") {
            if (serverNameSeen)
                Logger::log(LOG_WARNING, "Duplicate 'server_name' in server block. The first one will be used.");
            serverNameSeen = true;
        }
        validateServerDirective(d, hasListen, hasServerName);
    }

    if (!hasListen)
        throw ConfigValidationError("Missing 'listen' directive in server block.");
    if (!hasServerName)
        Logger::log(LOG_WARNING, "Missing 'server_name' directive in server block.");
    for (const Block& child : block.children) {
        if (child.name == "server")
            throw ConfigValidationError("Nested 'server' block is not allowed.");
        if (child.name != "location") {
            Logger::log(LOG_WARNING, "Unknown block '" + child.name + "' inside server.");
            continue;
        }
        validateLocationBlock(child);
    }
}

void ConfigValidator::validate(const Config& config) {
    if (config.blocks.empty()) {
        std::cerr << "Error: configuration file is empty.\n";
        return;
    }

    for (const Block& block : config.blocks) {
        if (block.name != "server") {
            std::cerr << "Warning: top-level block should be 'server'. Found '" << block.name << "'.\n";
            continue;
        }
        validateServerBlock(block);
    }
}


