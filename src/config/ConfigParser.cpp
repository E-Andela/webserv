#include "Config/ConfigParser.hpp"
#include "Config/Tokenizer.hpp"
#include "Config/ConfigError.hpp"
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

Block ConfigParser::parseBlock(Tokenizer& tokenizer, const std::string& name, std::vector<std::string> args) {
    Block block;
    block.name = name;
    block.args = args;

    while (true) {
        Token token = tokenizer.peek();

        // Detect unclosed block (EOF before closing brace)
        if (token.type == TokenType::END_OF_FILE) {
            throw ConfigParseError("Unexpected end of file: unclosed block '" + name + "'");
        }

        if (token.value == "}") {
            tokenizer.next(); // consume }
            break;
        }

        token = tokenizer.next();
        if (token.type != TokenType::WORD)
            continue;

        std::string directiveName = token.value;
        std::vector<std::string> directiveArgs;

        while (true) {
            Token next = tokenizer.peek();

            if (next.type == TokenType::END_OF_FILE) {
                throw ConfigParseError("Unexpected end of file while reading directive '" + directiveName + "'");
            }

            if (next.value == "{" || next.value == ";" || next.value == "}")
                break;

            token = tokenizer.next();
            directiveArgs.push_back(token.value);
        }

        Token next = tokenizer.peek();

        if (next.type == TokenType::END_OF_FILE) {
            throw ConfigParseError("Unexpected end of file after directive: " + directiveName);
        }

        if (next.value == "{") {
            tokenizer.next(); // consume {
            Block child = parseBlock(tokenizer, directiveName, directiveArgs);
            block.children.push_back(child);
        } else if (next.value == ";") {
            tokenizer.next(); // consume ;
            block.directives.push_back(Directive{directiveName, directiveArgs});
        } else {
            throw ConfigParseError("Unexpected token inside block: " + next.value);
        }
    }

    return block;
}




Config ConfigParser::parse(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw ConfigParseError("Could not open config file: " + path);
    }
    Tokenizer tokenizer(file);
    Config config;

    while (true) {
        Token token = tokenizer.peek();
        if (token.type == TokenType::END_OF_FILE)
            break;
        token = tokenizer.next();
        if (token.type != TokenType::WORD)
            continue;
        
        std::string name = token.value;
        std::vector<std::string> args;

        // read args until { or ;
        while (true) {
            Token next = tokenizer.peek();
            if (next.value == "{" || next.value == ";")
                break;
            token = tokenizer.next();
            args.push_back(token.value);
        }

        Token next = tokenizer.peek();
        if (next.value == "{") {
            tokenizer.next(); // consume {
            Block block = parseBlock(tokenizer, name, args);
            config.blocks.push_back(block);
        }
        else if (next.value == ";") {
            tokenizer.next(); //consume ;
            Directive directive{name, args};
            // Block& root = config.blocks.emplace_back();
            config.blocks.emplace_back();
            Block& root = config.blocks.back();

            root.name = "global";
            root.directives.push_back(directive);
        } else
            throw ConfigParseError("Expected '{' or ';' after directive: " + name);
    }

    return config;
}

