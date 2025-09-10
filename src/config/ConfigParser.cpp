#include "ConfigParser.hpp"
#include "Tokenizer.hpp"
#include "ConfigError.hpp"
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>
#include <set>

Block ConfigParser::parseBlock(Tokenizer& tokenizer,
                               const std::string& name,
                               std::vector<std::string> args)
{
    // Known directive/block starters we can see at this scope.
    // Used to detect:   listen 8080 server_name example.com;
    // where ';' after 8080 is missing.
    static const std::set<std::string> kKnownNames = {
        "listen","server_name","root","index","error_page","client_max_body_size",
        "methods","upload_path","return","cgi_path","cgi_extension","location",
        // If you later add more directives, list them here too.
    };

    auto joinArgs = [](const std::string& head,
                       const std::vector<std::string>& tail) -> std::string {
        std::string out = head;
        for (size_t i = 0; i < tail.size(); ++i) {
            out += (tail[i].empty() ? "" : " ");
            out += tail[i];
        }
        return out;
    };

    Block block;
    block.name = name;
    block.args = args;

    while (true) {
        Token look = tokenizer.peek();

        // Detect unclosed block (EOF before closing brace)
        if (look.type == TokenType::END_OF_FILE) {
            throw ConfigParseError("Unexpected end of file: unclosed block '" + name + "'");
        }

        if (look.type == TokenType::SYMBOL && look.value == "}") {
            tokenizer.next(); // consume '}'
            break;
        }

        // Consume the next token which should be the directive name (WORD)
        Token tok = tokenizer.next();
        if (tok.type != TokenType::WORD) {
            // Skip non-word noise (comments/whitespace handled earlier),
            // but if it's invalid, fail fast.
            if (tok.type == TokenType::INVALID) {
                throw ConfigParseError("Invalid token '" + tok.value + "' inside block '" + name + "'");
            }
            continue;
        }

        std::string directiveName = tok.value;
        std::vector<std::string> directiveArgs;

        // Collect directive arguments until we hit '{' (start child block) or ';' (end of directive)
        while (true) {
            Token next = tokenizer.peek();

            if (next.type == TokenType::END_OF_FILE) {
                throw ConfigParseError("Unexpected end of file while reading directive '" + directiveName + "'");
            }

            // End of this directive’s argument list
            if (next.type == TokenType::SYMBOL &&
                (next.value == "{" || next.value == ";" || next.value == "}")) {
                break;
            }

            // Heuristic: if the next token is a WORD that looks like another directive/block
            // starter, but we haven't seen ';' or '{' yet, it's almost certainly a missing ';'.
            if (next.type == TokenType::WORD && kKnownNames.count(next.value) > 0) {
                std::string sofar = joinArgs(directiveName, directiveArgs);
                throw ConfigParseError(
                    "Missing ';' after '" + sofar + "' (before '" + next.value + "')");
            }

            // Otherwise, consume the token as part of this directive's arguments.
            tok = tokenizer.next();
            if (tok.type == TokenType::INVALID) {
                throw ConfigParseError("Invalid token '" + tok.value + "' in directive '" + directiveName + "'");
            }
            // WORD or STRING are both acceptable as argument pieces
            directiveArgs.push_back(tok.value);
        }

        // Decide based on the delimiter we stopped at
        Token delim = tokenizer.peek();
        if (delim.type == TokenType::END_OF_FILE) {
            throw ConfigParseError("Unexpected end of file after directive: " + directiveName);
        }

        if (delim.type == TokenType::SYMBOL && delim.value == "{") {
            tokenizer.next(); // consume '{'
            Block child = parseBlock(tokenizer, directiveName, directiveArgs);
            block.children.push_back(child);
        } else if (delim.type == TokenType::SYMBOL && delim.value == ";") {
            tokenizer.next(); // consume ';'
            block.directives.push_back(Directive{directiveName, directiveArgs});
        } else if (delim.type == TokenType::SYMBOL && delim.value == "}") {
            // This can happen if someone wrote:
            //   directive arg arg
            // and closed the block without a ';'
            std::string sofar = joinArgs(directiveName, directiveArgs);
            throw ConfigParseError("Missing ';' after '" + sofar + "' before '}'");
        } else {
            // Shouldn't get here, but keep a guard.
            throw ConfigParseError("Unexpected token inside block: " + delim.value);
        }
    }

    return block;
}


// Block ConfigParser::parseBlock(Tokenizer& tokenizer, const std::string& name, std::vector<std::string> args) {
//     Block block;
//     block.name = name;
//     block.args = args;

//     while (true) {
//         Token token = tokenizer.peek();

//         // Detect unclosed block (EOF before closing brace)
//         if (token.type == TokenType::END_OF_FILE) {
//             throw ConfigParseError("Unexpected end of file: unclosed block '" + name + "'");
//         }

//         if (token.value == "}") {
//             tokenizer.next(); // consume }
//             break;
//         }

//         token = tokenizer.next();
//         if (token.type != TokenType::WORD)
//             continue;

//         std::string directiveName = token.value;
//         std::vector<std::string> directiveArgs;

//         while (true) {
//             Token next = tokenizer.peek();

//             if (next.type == TokenType::END_OF_FILE) {
//                 throw ConfigParseError("Unexpected end of file while reading directive '" + directiveName + "'");
//             }

//             if (next.value == "{" || next.value == ";" || next.value == "}")
//                 break;

//             token = tokenizer.next();
            
//             directiveArgs.push_back(token.value);
//         }

//         Token next = tokenizer.peek();

//         if (next.type == TokenType::END_OF_FILE) {
//             throw ConfigParseError("Unexpected end of file after directive: " + directiveName);
//         }

//         if (next.value == "{") {
//             tokenizer.next(); // consume {
//             Block child = parseBlock(tokenizer, directiveName, directiveArgs);
//             block.children.push_back(child);
//         } else if (next.value == ";") {
//             tokenizer.next(); // consume ;
//             block.directives.push_back(Directive{directiveName, directiveArgs});
//         } else {
//             throw ConfigParseError("Unexpected token inside block: " + next.value);
//         }
//     }

//     return block;
// }




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

