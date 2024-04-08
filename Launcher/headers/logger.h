#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <iomanip>
#include <Windows.h>
#include <fstream>

enum LogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
};

class Logger {
public:
    const std::vector<std::string> levelStrings{
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
    };

    Logger();
    ~Logger();
    bool IsDebugLevel();
    void SetFile(const std::filesystem::path& fileName);
    void SetMinLevel(LogLevel level);
    void Clear() const;
    void Write(LogLevel level, const std::string& text) const;
    void Write(LogLevel level, const char* fmt, ...) const;

    std::filesystem::path GetFolder() { return file.parent_path(); }

private:
    std::filesystem::path file = "";
    std::string levelText(LogLevel level) const;
    LogLevel minLevel = LOG_INFO;
};

extern Logger logger;