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
    g_Config.Setup(ctx.GetFolder());
    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

void Core::SetupLogger() {
    const std::string logPath = ctx.GetFolder() + "\\" + "Logs";
    std::string msg = "Failed to create Logs directory:\n" + logPath + "\nError: ";
    bool failed = false;

    try {
        if (!fs::is_directory(logPath) || !fs::exists(logPath)) {
            if (!fs::create_directory(logPath)) {
                msg += "Unknown";
                failed = true;
            }
        }
    }
    catch (fs::filesystem_error const& e) {
        msg += std::string(e.what());
        
        failed = true;
    }

    if (failed)
        MessageBox(NULL, msg.c_str(), "WARNING", MB_ICONWARNING);

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