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

    g_Config.Load();

    logger.Write(LOG_INFO, "[%s] %s %s", __FUNCTION__, TOOL_NAME, TOOL_VERSION);
    logger.Write(LOG_INFO, "[%s] Game Install Dir: %s", __FUNCTION__, GetGameInstallDir().c_str());

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

const char* Core::GetToolVer() {
    return TOOL_VERSION;
}

std::string Core::GetGameInstallDir() {
    //Computer\HKEY_LOCAL_MACHINE\SOFTWARE\EA Sports\FIFA 22
    DWORD dwType = REG_SZ;
    HKEY hKey = 0;
    std::string subkey = std::string("SOFTWARE\\EA Sports\\FIFA ") + std::to_string(FIFA_EDITION);

    RegOpenKey(HKEY_LOCAL_MACHINE, subkey.c_str(), &hKey);
    
    const char* val_name = "Install Dir";

    char value_buf[1024];
    DWORD value_length = sizeof(value_buf);
    RegQueryValueEx(hKey, val_name, NULL, &dwType, reinterpret_cast<LPBYTE>(value_buf), &value_length);
    return std::string(value_buf);
}

void Core::RunGame() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    std::string proc_name = g_Config.proc_names[0];

    if (g_Config.is_trial) proc_name = g_Config.proc_names[1];

    std::string game_full_path = GetGameInstallDir() + proc_name;

    logger.Write(LOG_INFO, "[%s] game_full_path: %s", __FUNCTION__, game_full_path.c_str());

    ShellExecute(NULL, "runas", game_full_path.c_str(), NULL, NULL, SW_SHOWDEFAULT);

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