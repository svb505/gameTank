#include "Logger.h"
#include <ctime>
#include <fstream>
#include <format>

static std::ofstream logFile;

void Logger::initLogger() { logFile.open("game_logs.log", std::ios::out | std::ios::app); }
void Logger::shutDown() { if (logFile.is_open()) logFile.close(); }
static const char* LevelToString(logLevel level) {
    switch (level) {
    case logLevel::Info:    return "[INFO]";
    case logLevel::Warning: return "[WARN]";
    case logLevel::Error:   return "[ERROR]";
    case logLevel::Critical:   return "[CRITICAL]";
    }
    return "[UNKNOWN]";
}
void Logger::Log(logLevel level, const std::string msg) {
    std::time_t t = std::time(nullptr);

    std::tm tm{};
    localtime_s(&tm, &t);

    char timeBuf[32];
    std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &tm);

    std::string final = std::format("{} {} {}", timeBuf, LevelToString(level), msg);

    if (logFile.is_open()) logFile << final << std::endl;
}