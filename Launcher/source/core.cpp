#include <core.h>

Core::Core()
{
}

Core::~Core()
{
}

void Core::Init()
{
    ctx.Update(GetModuleHandle(NULL));

    SetupLogger();
    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

void Core::SetupLogger() {
    const std::string logPath = ctx.GetFolder() + "\\" + "Logs";
    if (!fs::is_directory(logPath) || !fs::exists(logPath)) {
        fs::create_directory(logPath);
    }

    SYSTEMTIME currTimeLog;
    GetLocalTime(&currTimeLog);
    std::ostringstream ssLogFile;
    ssLogFile << "log_launcher_" <<
        std::setw(2) << std::setfill('0') << currTimeLog.wDay << "-" <<
        std::setw(2) << std::setfill('0') << currTimeLog.wMonth << "-" <<
        std::setw(4) << std::setfill('0') << currTimeLog.wYear << ".txt";
    const std::string logFile = logPath + "\\" + ssLogFile.str();
    logger.SetFile(logFile);
}

Core g_Core;