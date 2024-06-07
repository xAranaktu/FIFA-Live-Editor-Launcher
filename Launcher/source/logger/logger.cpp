#pragma once
#include "logger/logger.h"

Logger::Logger() {}

Logger::~Logger() {}

bool Logger::IsDebugLevel() {
    return minLevel == LOG_LEVEL::LOG_DEBUG;
}

void Logger::SetFile(const std::filesystem::path& fileName) {
    fp = fileName;
    Reopen();
}

void Logger::SetMinLevel(LOG_LEVEL level) {
    minLevel = level;
}

void Logger::SetMinLevel(int level) {
    minLevel = static_cast<LOG_LEVEL>(level);
}

void Logger::SetOnWriteCallback(std::function<void(LOG_LEVEL, std::string, std::string)> _f) {
    onWriteCallback = _f;
}


void Logger::Write(LOG_LEVEL level, const std::string& text) {
    if (level < minLevel) return;

    std::string ts = GetTime();

    std::string out;
    out.reserve(text.length() + 32);

    out.append(ts);
    out.push_back('\t');

    out.append(GetLevelName(level));
    out.push_back('\t');

    out.append(text);
    out.push_back('\n');

    lock.lock();

    if (onWriteCallback) {
        onWriteCallback(level, ts, text);
    }

    logFile << out;
    logFile.flush();

    lock.unlock();

    Reopen();
}

std::string Logger::GetLevelName(LOG_LEVEL level) {
    return logger_level_strings.at(static_cast<int>(level));
}

std::string Logger::GetTime() {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm gmt{};

    gmtime_s(&gmt, &tt);

    std::chrono::duration<double> fractional_seconds = (tp - std::chrono::system_clock::from_time_t(tt)) + std::chrono::seconds(gmt.tm_sec);
    // format the string
    std::string buffer("hr:mn:sc.xxxxxx0");
    int ret = snprintf(
        &buffer.front(), buffer.length(),
        "%02d:%02d:%09.6f",
        gmt.tm_hour, gmt.tm_min, fractional_seconds.count()
    );

    buffer.pop_back();
    return buffer;
}

void Logger::Reopen() {
    auto now = std::chrono::system_clock::now();
    lock.lock();
    if (now - last_reopen > reopen_interval) {
        last_reopen = now;
        try {
            logFile.close();
        }
        catch (...) {}
        try {
            const auto parent_dir = std::filesystem::path(fp).parent_path();
            if (!std::filesystem::is_directory(parent_dir)) {
                if (!std::filesystem::create_directories(parent_dir)) {
                    throw std::runtime_error("Cannot create directory for log file: " + parent_dir.string());
                }
            }
            logFile.open(fp, std::ofstream::out | std::ofstream::app);
            if (logFile.fail()) {
                throw std::runtime_error("Cannot create log file");
            }
            last_reopen = std::chrono::system_clock::now();
        }
        catch (std::exception& e) {
            try {
                logFile.close();
            }
            catch (...) {}
            throw e;
        }
    }
    lock.unlock();
}

// Everything's gonna use this instance.
Logger defaultLogger;