#pragma once
#include <logger.h>

Logger::Logger() {}

Logger::~Logger() {}

bool Logger::IsDebugLevel() {
    return minLevel == LOG_DEBUG;
}

void Logger::SetFile(const std::string& fileName) {
    file = fileName;
}

void Logger::SetMinLevel(LogLevel level) {
    minLevel = level;
}

void Logger::Clear() const {
    std::ofstream logFile(file, std::ofstream::out | std::ofstream::trunc);
}

void Logger::Write(LogLevel level, const std::string& text) const {
    if (level < minLevel) return;

    std::ofstream logFile(file, std::ios_base::out | std::ios_base::app);
    SYSTEMTIME currTimeLog;
    GetLocalTime(&currTimeLog);
    logFile << "[" <<
        std::setw(2) << std::setfill('0') << currTimeLog.wHour << ":" <<
        std::setw(2) << std::setfill('0') << currTimeLog.wMinute << ":" <<
        std::setw(2) << std::setfill('0') << currTimeLog.wSecond << "." <<
        std::setw(3) << std::setfill('0') << currTimeLog.wMilliseconds << "] " <<
        text;
}

void Logger::Write(LogLevel level, const char* fmt, ...) const {
    if (level < minLevel) return;
    const __int64 size = 1024 * 5; // 5kb
    char* logbuff = new char[size];

    __int64 offset = sprintf_s(logbuff, size,
        "[%s] ",
        levelText(level).c_str()
    );

    va_list args;
    va_start(args, fmt);
    offset += vsnprintf(logbuff + offset, size - offset - 1, fmt, args);
    va_end(args);
    sprintf_s(logbuff + offset, size - offset - 1, "%s", "\n");

    Write(level, std::string(logbuff));
    delete[] logbuff;
}

std::string Logger::levelText(LogLevel level) const {
    return levelStrings[level];
}

// Everything's gonna use this instance.
Logger logger;