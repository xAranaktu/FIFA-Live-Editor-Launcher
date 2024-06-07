#pragma once
#include <string>
#include <mutex>
#include <functional>
#include <filesystem>
#include <vector>
#include <iomanip>
#include <Windows.h>
#include <fstream>
#include <source_location>

#include <logger/const.h>
#include <logger/enum.h>


class Logger;

#ifndef LOG_DEBUG
#ifndef LOG_INFO
#ifndef LOG_WARN
#ifndef LOG_ERROR
#ifndef LOG_FATAL

#define LOG_DEBUG(x)            defaultLogger.Write(LOG_LEVEL::LOG_DEBUG, x)
#define LOG_INFO(x)             defaultLogger.Write(LOG_LEVEL::LOG_INFO, x)
#define LOG_WARN(x)             defaultLogger.Write(LOG_LEVEL::LOG_WARN, x)
#define LOG_ERROR(x)            defaultLogger.Write(LOG_LEVEL::LOG_ERROR, x)
#define LOG_FATAL(x)            defaultLogger.Write(LOG_LEVEL::LOG_FATAL, x)


#endif
#endif
#endif
#endif
#endif

#ifndef LOG_FUNC_START
#ifndef LOG_FUNC_END

#define LOG_FUNC_START()       LOG_INFO(std::format("[{}] Start", __FUNCTION__));
#define LOG_FUNC_END()         LOG_INFO(std::format("[{}] Done", __FUNCTION__));

#endif
#endif


#ifndef LOG_DEV_DEBUG
#ifndef LOG_DEV_INFO
#ifndef LOG_DEV_WARN
#ifndef LOG_DEV_ERROR
#ifndef LOG_DEV_FATAL

#ifdef IS_DEV
#define LOG_DEV_DEBUG(x)        defaultLogger.Write(LOG_LEVEL::LOG_DEBUG, std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + x)
#define LOG_DEV_INFO(x)         defaultLogger.Write(LOG_LEVEL::LOG_INFO, std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + x)
#define LOG_DEV_WARN(x)         defaultLogger.Write(LOG_LEVEL::LOG_WARN, std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + x)
#define LOG_DEV_ERROR(x)        defaultLogger.Write(LOG_LEVEL::LOG_ERROR, std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + x)
#define LOG_DEV_FATAL(x)        defaultLogger.Write(LOG_LEVEL::LOG_FATAL, std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + x)
#else
#define LOG_DEV_DEBUG(x);
#define LOG_DEV_INFO(x);
#define LOG_DEV_WARN(x);
#define LOG_DEV_ERROR(x);
#define LOG_DEV_FATAL(x);
#endif 

#endif
#endif
#endif
#endif
#endif


class Logger {
public:
    Logger();
    ~Logger();
    bool IsDebugLevel();
    void SetFile(const std::filesystem::path& fileName);
    void SetMinLevel(LOG_LEVEL level);
    void SetMinLevel(int level);
    void SetOnWriteCallback(std::function<void(LOG_LEVEL, std::string, std::string)> _f);

    void Write(LOG_LEVEL level, const std::string& text);
    std::filesystem::path GetFolder() { return fp.parent_path(); };
    int GetLogLevel() { return static_cast<int>(minLevel); }

protected:
    std::mutex lock;

    void Reopen();

private:
    std::function<void(LOG_LEVEL, std::string, std::string)> onWriteCallback;

    LOG_LEVEL minLevel = LOG_LEVEL::LOG_INFO;
    std::filesystem::path fp = "";

    std::ofstream logFile;
    std::chrono::seconds reopen_interval;
    std::chrono::system_clock::time_point last_reopen;

    std::string GetLevelName(LOG_LEVEL level);
    std::string GetTime();
};

extern Logger defaultLogger;