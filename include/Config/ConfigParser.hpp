#pragma once

#include "AST.hpp"
#include "Tokenizer.hpp"
#include <string>
#include <vector>

class ConfigParser {

private:
    static Block parseBlock(Tokenizer& tokenizer, const std::string& name, std::vector<std::string> args);

    /* Helper functions */
    

public:
    static Config parse(const std::string& path);
};
