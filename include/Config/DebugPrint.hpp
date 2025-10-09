#pragma once
#include "AST.hpp"
#include "ServerConfig.hpp"

void printBlock(const Block& block, int indent = 0);
void printASTTree(const Config& config);
void printMappedConfig(const std::vector<ServerConfig>& servers);
