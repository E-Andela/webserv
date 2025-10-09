#include "Config/Logger.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>


std::string Logger::getTimestamp() {
    std::time_t now;
    now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return std::string(buffer);
}

std::string Logger::levelToString(LogLevel level) {
    switch(level) {
        case LOG_INFO: return "INFO";
        case LOG_ERROR: return "ERROR";
        case LOG_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& msg) {
#ifndef ENABLE_LOG_DEBUG
    if (level == LOG_DEBUG)
        return;
#endif

    std::cout << "[" << getTimestamp() << "] "
              << "[" << levelToString(level) << " ]"
              << msg << std::endl;
}
