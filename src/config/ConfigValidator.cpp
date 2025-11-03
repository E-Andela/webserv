#include "Config/ConfigValidator.hpp"
#include "Config/Logger.hpp"
#include "Config/ConfigError.hpp"
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


/* valid methods */
static const std::set<std::string> validMethods = {"HEAD", "GET", "POST", "DELETE"};

/*
validate presense of mandatory fields
TODO: define mandatory fields in docs
*/
static void validateServerDirective(const Directive& d, bool& hasListen, bool& hasServerName) {

    // checks directive's name
    if (serverDirectives.find(d.name) == serverDirectives.end()) {
        Logger::log(LOG_ERROR, "Warning: unknown directive '" + d.name + "' in server block.");
        return;
    }

    if (d.name == "listen") {
        hasListen = true;
        if (d.args.empty())
            Logger::log(LOG_ERROR, "'listen' directive missing argument");
        else {
            int port;
            try {
                if (!parse_port(d.args[0], port))
                    Logger::log(LOG_ERROR, "Invalid port in 'listen'");
            } catch (const std::exception& e) {
                Logger::log(LOG_ERROR, "Failed to parse port: " + std::string(e.what()));
            }
        }
    }
    
    else if (d.name == "server_name") {
        hasServerName = true;
        if (d.args.empty())
            Logger::log(LOG_WARNING, "'server_name' is empty.");
    } else if ((d.name == "root" || d.name == "index") && d.args.empty())
        Logger::log(LOG_ERROR, "No value for: " + d.name +".");
    else if (d.name == "error_page") {
        if (d.args.size() != 2 || !is_digit_string(d.args[0]))
            Logger::log(LOG_ERROR, "'error_page' must be: error_page <code> <path>;");
    }
    // validate more 
}


/* valid Location directives */
static const std::set<std::string> locationDirectives = {
    "methods", "upload_path", "return",
    "cgi_path", "cgi_extension", "client_max_body_size"
};

static void validateLocationDirective(const Directive& d) {
    if (locationDirectives.find(d.name) == locationDirectives.end()) {
        Logger::log(LOG_WARNING, "Unknown directive '" + d.name + "'."); // add locationpath for better logging
        return;
    }

    if ((d.name == "upload_path" || d.name == "cgi_path" || d.name == "cgi_extension") && d.args.empty()) {
        Logger::log(LOG_ERROR, "'" + d.name + "' requires a value.");
    }

    if (d.name == "return") {
        if (d.args.empty()) {
            Logger::log(LOG_ERROR, "'return' directive requires a status code in location '");
        } else if (!is_digit_string(d.args[0])) {
            Logger::log(LOG_ERROR, "'return' code must be numeric.");
        }
    }

    if (d.name == "methods") {
        if (d.args.empty()) {
            Logger::log(LOG_ERROR, "'methods' must list at least one method.");
        } else {
            for (const std::string& m : d.args) {
                if (validMethods.find(m) == validMethods.end()) {
                    Logger::log(LOG_ERROR, "Invalid HTTP method '" + m + "'.");
                }
            }
        }
    }
}



static void validateLocationBlock(const Block& block) {
    if (block.args.empty()) {
        Logger::log(LOG_ERROR, "location block missing path.");
        return;
    }

    bool hasMethods = false;
    for (const Directive& d : block.directives) {
        if (d.name == "methods") hasMethods = true;
        validateLocationDirective(d);
    }

    if (!hasMethods) {
        Logger::log(LOG_WARNING,
            "location '" + block.args[0] + "' has no 'methods' directive.");
    }
       // Disallow nested blocks inside location (only directives expected)
    for (const Block& child : block.children) {
        Logger::log(LOG_WARNING,
            "unknown nested block '" + child.name + "' inside location '" + block.args[0] + "'.");
    }
}

static void validateServerBlock(const Block& block) {
    bool hasListen = false;
    bool hasServerName = false;

    for (const Directive& d : block.directives)
        validateServerDirective(d, hasListen, hasServerName);

    if (!hasListen)
        std::cerr << "Error: missing 'listen' directive.\n";
    if (!hasServerName)
        std::cerr << "Warning: missing 'server_name' directive.\n";

    for (const Block& child : block.children) {
        if (child.name != "location") {
            std::cerr << "Warning: unknown block '" << child.name << "' inside server.\n";
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
