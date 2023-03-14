#include <core.h>

Core::Core()
{
}

Core::~Core()
{
}

bool Core::Init()
{
    ctx.Update(GetModuleHandle(NULL));

    SetupLogger();
    g_Config.Setup(ctx.GetFolder());

    g_Config.Load();

    std::filesystem::path game_install_dir = GetGameInstallDir();
    std::filesystem::path le_dir = ctx.GetFolder();

    logger.Write(LOG_INFO, "[%s] %s %s", __FUNCTION__, TOOL_NAME, TOOL_VERSION);
    logger.Write(LOG_INFO, "[%s] Game Install Dir: %s", __FUNCTION__, game_install_dir.c_str());
    logger.Write(LOG_INFO, "[%s] Live Editor Dir: %s", __FUNCTION__, le_dir.string().c_str());

    // std::string app_data("AppData\\Local\\Temp");
    // if (le_dir.find(app_data) != std::string::npos) {
    //     logger.Write(LOG_ERROR, "ERROR: Not extracted");
    // 
    //     MessageBox(NULL, "Archive not extracted\n\nUnpack live editor with winrar or alternative software if you want to use it", "Not extracted", MB_ICONERROR);
    //     return false;
    // }
    // 
    // if (!isASCII(game_install_dir)) {
    //     logger.Write(LOG_ERROR, "ERROR: Invalid chars in game install dir");
    // 
    //     MessageBox(NULL, "Your game installation directory contains non-English character(s) which are not supported by the Live Editor.\n\nSolution:\nMove game to other path with English only characters.", "INVALID CHARACTER", MB_ICONERROR);
    //     return false;
    // }
    // 
    // if (!isASCII(le_dir)) {
    //     logger.Write(LOG_ERROR, "ERROR: Invalid chars in live editor dir");
    // 
    //     MessageBox(NULL, "Live Editor installation directory contains non-English character(s) which are not supported.\n\nSolution:\nDownload Live Editor again and put it in path with English only characters.", "INVALID CHARACTER", MB_ICONERROR);
    //     return false;
    // }
    RestoreOrgGameFiles();
    BackupOrgGameFiles();
    CopyFakeEAAC();

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);

    return true;
}

void Core::onExit() {
    logger.Write(LOG_INFO, "[%s] Waiting for EAAntiCheat.GameServiceLauncher.exe", __FUNCTION__);

    int attempts = 0;
    DWORD pid = 0;

    do
    {
        pid = 0;
        PVOID snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 process;
        process.dwSize = sizeof(process);
        while (Process32Next(snapshot, &process)) {
            if (strcmp(process.szExeFile, "EAAntiCheat.GameServiceLauncher.exe") == 0) {
                pid = process.th32ProcessID;
                break;
            }
        }
        CloseHandle(snapshot);
        Sleep(1000);
    } while (pid > 0);

    logger.Write(LOG_INFO, "[%s] Trying to restore", __FUNCTION__);
    RestoreOrgGameFiles();
}

const char* Core::GetToolVer() {
    return TOOL_VERSION;
}

fs::path Core::GetGameInstallDir() {
    //Computer\HKEY_LOCAL_MACHINE\SOFTWARE\EA Sports\FIFA 22
    DWORD dwType = REG_SZ;
    HKEY hKey = 0;
    std::string subkey = std::string("SOFTWARE\\EA Sports\\FIFA ") + std::to_string(FIFA_EDITION);

    LSTATUS open_status = RegOpenKey(HKEY_LOCAL_MACHINE, subkey.c_str(), &hKey);
    if (open_status != ERROR_SUCCESS) {
        logger.Write(LOG_ERROR, "[%s] RegOpenKey failed %d", __FUNCTION__, open_status);
    }
    
    const char* val_name = "Install Dir";

    char value_buf[1024];
    DWORD value_length = sizeof(value_buf);
    LSTATUS query_status = RegQueryValueEx(hKey, val_name, NULL, &dwType, reinterpret_cast<LPBYTE>(value_buf), &value_length);

    if (query_status != ERROR_SUCCESS) {
        logger.Write(LOG_ERROR, "[%s] RegQueryValueEx failed %d", __FUNCTION__, query_status);
        return fs::path("");
    }

    return fs::path(value_buf);
}

void Core::RunGame() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    std::string proc_name = g_Config.proc_names[0];
    std::wstring params = L"";

    if (g_Config.is_trial) {
        proc_name = g_Config.proc_names[1];
        params = L"-trial";
    }

    std::filesystem::path game_full_path = GetGameInstallDir() / proc_name;

    if (fs::exists(game_full_path)) {
        logger.Write(LOG_INFO, "[%s] game_full_path: %s", __FUNCTION__, game_full_path.c_str());

        ShellExecuteW(NULL, L"runas", game_full_path.c_str(), params.c_str(), NULL, SW_SHOWDEFAULT);

        logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
    }
    else {
        std::wstring msg = L"Can't Find File:\n" + game_full_path.wstring();

        MessageBoxW(NULL, msg.c_str(), L"ERROR", MB_ICONERROR);
    }
}

void Core::SetupLogger() {
    const std::filesystem::path logPath = ctx.GetFolder() / "Logs";
    std::string msg = "Failed to create Logs directory:\n" + logPath.string() + "\nError: ";
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
    const std::filesystem::path logFile = logPath / ssLogFile.str();
    logger.SetFile(logFile);
}

bool Core::isASCII(const std::string& s) {
    return true;
    //return !std::any_of(s.begin(), s.end(), [](char c) {
    //    return static_cast<unsigned char>(c) > 127;
    //});
}

fs::path Core::GetEAACLauncherPath() {
    fs::path result = GetGameInstallDir();
    if (result.empty()) return result;

    return result / "EAAntiCheat.GameServiceLauncher.exe";
}

fs::path Core::GetFakeEAACPath() {
    return  ctx.GetFolder() / "FakeEAACLauncher" / "EAAntiCheat.GameServiceLauncher.exe";
}

fs::path Core::GetBAKPathFor(fs::path p) {
    std::string bak_fname = p.filename().string() + ".backup";
    return p.parent_path() / bak_fname;
}

void Core::CopyFakeEAAC() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);
    const std::filesystem::path fake_eaac_path = GetFakeEAACPath();

    try {
        fs::path eaac_path = GetEAACLauncherPath();
        if (!eaac_path.empty()) {
            fs::path bak = GetBAKPathFor(eaac_path);
            if (fs::exists(bak)) {
                if (fs::exists(eaac_path))    fs::remove(eaac_path);

                fs::copy(fake_eaac_path, eaac_path);
            }
            else {
                logger.Write(LOG_ERROR, "[%s] Can't find eaac backup", __FUNCTION__);
            }
        }
    }
    catch (fs::filesystem_error const& e) {
        logger.Write(LOG_ERROR, "[%s] Restore EAAC error %s", __FUNCTION__, e.what());
    }

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

void Core::BackupOrgGameFiles() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    try {
        std::filesystem::path eaac_path = GetEAACLauncherPath();
        if (!eaac_path.empty()) {
            fs::path bak = GetBAKPathFor(eaac_path);
            if (fs::exists(bak))    fs::remove(bak);

            fs::copy(eaac_path, bak);
        }
    }
    catch (fs::filesystem_error const& e) {
        logger.Write(LOG_ERROR, "[%s] Backup EAAC error %s", __FUNCTION__, e.what());
    }

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}
void Core::RestoreOrgGameFiles() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    try {
        std::filesystem::path eaac_path = GetEAACLauncherPath();
        if (!eaac_path.empty()) {
            fs::path bak = GetBAKPathFor(eaac_path);
            if (fs::exists(bak)) {
                if (fs::exists(eaac_path))    fs::remove(eaac_path);

                fs::copy(bak, eaac_path);
                fs::remove(bak);
            }
        }
    }
    catch (fs::filesystem_error const& e) {
        logger.Write(LOG_ERROR, "[%s] Restore EAAC error %s", __FUNCTION__, e.what());
    }

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

Core g_Core;