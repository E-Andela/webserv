#pragma once
#include <string>

enum LogLevel {
    LOG_INFO,
    LOG_ERROR,
    LOG_DEBUG
};

// logging reasons - look at nginxs's stuff , logging  - user level or god level
class Logger {

private:
    static std::string getTimestamp();
    static std::string levelToString(LogLevel level);

public:
    static void log(LogLevel level, const std::string& msg);
};
