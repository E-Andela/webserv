#pragma once

#include <string>
#include <vector>

struct Directive {
    std::string name;
    std::vector<std::string> args;
};

struct Block {
    std::string name;
    std::vector<std::string> args;
    std::vector<Directive> directives;
    std::vector<Block> children;
};

struct Config {
    std::vector<Block> blocks;
};
