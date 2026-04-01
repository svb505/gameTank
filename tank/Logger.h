#pragma once
#include <cstring>
#include <string>

#define LOG_INFO(msg)  Logger::Log(logLevel::Info, msg)
#define LOG_WARN(msg)  Logger::Log(logLevel::Warning, msg)
#define LOG_ERROR(msg) Logger::Log(logLevel::Error, msg)

enum class logLevel {
	Info,
	Warning,
	Error,
	Critical
};

class Logger {
public:
	static void initLogger();
	static void Log(logLevel level, const std::string msg);
	static void shutDown();
};
