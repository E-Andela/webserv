#pragma once

#include "AST.hpp"
#include <string>
class ConfigValidator {
public:
    static void validate(const Config& config);
};
