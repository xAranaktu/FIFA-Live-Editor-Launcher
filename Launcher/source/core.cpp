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

    std::string game_install_dir = GetGameInstallDir();
    std::string le_dir = ctx.GetFolder();

    logger.Write(LOG_INFO, "[%s] %s %s", __FUNCTION__, TOOL_NAME, TOOL_VERSION);
    logger.Write(LOG_INFO, "[%s] Game Install Dir: %s", __FUNCTION__, game_install_dir.c_str());
    logger.Write(LOG_INFO, "[%s] Live Editor Dir: %s", __FUNCTION__, le_dir.c_str());


    std::string app_data("AppData\\Local\\Temp");
    if (le_dir.find(app_data) != std::string::npos) {
        logger.Write(LOG_ERROR, "ERROR: Not extracted");

        MessageBox(NULL, "Archive not extracted\n\nUnpack live editor with winrar or alternative software if you want to use it", "Not extracted", MB_ICONERROR);
        return false;
    }

    if (!isASCII(game_install_dir)) {
        logger.Write(LOG_ERROR, "ERROR: Invalid chars in game install dir");

        MessageBox(NULL, "Your game installation directory contains non-English character(s) which are not supported by the Live Editor.\n\nSolution:\nMove game to other path with English only characters.", "INVALID CHARACTER", MB_ICONERROR);
        return false;
    }

    if (!isASCII(le_dir)) {
        logger.Write(LOG_ERROR, "ERROR: Invalid chars in live editor dir");

        MessageBox(NULL, "Live Editor installation directory contains non-English character(s) which are not supported.\n\nSolution:\nDownload Live Editor again and put it in path with English only characters.", "INVALID CHARACTER", MB_ICONERROR);
        return false;
    }
    RestoreOrgGameFiles();
    BackupOrgGameFiles();
    ApplyPlatform(g_Config.platform_origin);

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);

    return true;
}

void Core::onExit() {
    RestoreOrgGameFiles();
}

const char* Core::GetToolVer() {
    return TOOL_VERSION;
}

std::string Core::GetGameInstallDir() {
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
        return std::string("");
    }

    return std::string(value_buf);
}

void Core::ToggleAnticheat(bool disable) {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    std::string installerdata_path = GetInstallerDataPath();
    logger.Write(LOG_INFO, "[%s] installerdata_path: %s", __FUNCTION__, installerdata_path.c_str());
    if (installerdata_path.empty()) {
        logger.Write(LOG_WARN, "[%s] can't find installerdata.xml", __FUNCTION__);
        return;
    }

    std::ifstream f_installerdata(installerdata_path.c_str());
    if (!f_installerdata) {
        logger.Write(LOG_WARN, "[%s] can't open installerdata.xml", __FUNCTION__);
        return;
    }
    std::vector<std::string> new_content;

    std::string line_trial = "      <filePath>[HKEY_LOCAL_MACHINE\\SOFTWARE\\EA Sports\\FIFA 23\\Install Dir]FIFA23_Trial.exe</filePath>";
    std::string line_full = "      <filePath>[HKEY_LOCAL_MACHINE\\SOFTWARE\\EA Sports\\FIFA 23\\Install Dir]FIFA23.exe</filePath>";

    if (!disable) {
        line_full = line_trial = "      <filePath>[HKEY_LOCAL_MACHINE\\SOFTWARE\\EA Sports\\FIFA 23\\Install Dir]EAAntiCheat.GameServiceLauncher.exe</filePath>";
    }

    std::string line_to_find = "<filePath>[HKEY_LOCAL_MACHINE";
    bool is_trial_line = true;
    for (std::string line; std::getline(f_installerdata, line); )
    {
        if (line.find(line_to_find) != std::string::npos) {
            // Needs replace
            if (is_trial_line) {
                new_content.push_back(line_trial);
                is_trial_line = false;
            }
            else {
                new_content.push_back(line_full);
            }
        }
        else {
            // No need to replace
            new_content.push_back(line);
        }
    }
    f_installerdata.close();

    std::ofstream f_modified_installerdata(installerdata_path.c_str(), std::ios::trunc | std::ios::binary);
    if (!f_modified_installerdata) {
        logger.Write(LOG_WARN, "[%s] can't open installerdata.xml to write", __FUNCTION__);
        return;
    }

    for (auto line : new_content) {
        f_modified_installerdata << line << "\n";
    }

    f_modified_installerdata.close();

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

void Core::RunGame() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    std::string proc_name = g_Config.proc_names[0];
    std::string params = "";

    if (g_Config.is_trial) {
        proc_name = g_Config.proc_names[1];
        params = "-trial";
    }

    std::string game_full_path = GetGameInstallDir() + proc_name;

    logger.Write(LOG_INFO, "[%s] game_full_path: %s", __FUNCTION__, game_full_path.c_str());

    ShellExecute(NULL, "runas", game_full_path.c_str(), params.c_str(), NULL, SW_SHOWDEFAULT);

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

void Core::ApplyPlatform(bool isOrigin) {
    if (isOrigin) {
        // Disable EAAC on Origin
        //ToggleAnticheat(true);
    }
    else {
        // Keep EAAC on EAApp, but use fake EAAntiCheat.GameServiceLauncher.exe
        //ToggleAnticheat(false);
        CopyFakeEAAC();
    }
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

bool Core::isASCII(const std::string& s) {
    return !std::any_of(s.begin(), s.end(), [](char c) {
        return static_cast<unsigned char>(c) > 127;
    });
}

std::string Core::GetInstallerDataPath() {
    return "";


    //std::string installerdata_path = GetGameInstallDir();
    //if (installerdata_path.empty()) return installerdata_path;
    
    //return installerdata_path + "__Installer\\installerdata.xml";
}
std::string Core::GetEAACLauncherPath() {
    std::string eaac_path = GetGameInstallDir();
    if (eaac_path.empty()) return eaac_path;

    return eaac_path + "EAAntiCheat.GameServiceLauncher.exe";
}

void Core::CopyFakeEAAC() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);
    const std::string fake_eaac_path = ctx.GetFolder() + "\\FakeEAACLauncher\\EAAntiCheat.GameServiceLauncher.exe";

    try {
        std::string eaac_path = GetEAACLauncherPath();
        if (!eaac_path.empty()) {
            std::string bak = eaac_path + ".backup";
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
        std::string installerdata_path = GetInstallerDataPath();
        if (!installerdata_path.empty()) {
            std::string bak = installerdata_path + ".backup";
            if (fs::exists(bak))    fs::remove(bak);

            fs::copy(installerdata_path, bak);
        }
    }
    catch (fs::filesystem_error const& e) {
        logger.Write(LOG_ERROR, "[%s] Backup installerdata error %s", __FUNCTION__, e.what());
    }

    try {
        std::string eaac_path = GetEAACLauncherPath();
        if (!eaac_path.empty()) {
            std::string bak = eaac_path + ".backup";
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
        std::string installerdata_path = GetInstallerDataPath();
        if (!installerdata_path.empty()) {
            std::string bak = installerdata_path + ".backup";
            if (fs::exists(bak)) {
                if (fs::exists(installerdata_path))    fs::remove(installerdata_path);

                fs::copy(bak, installerdata_path);
                fs::remove(bak);
            }

        }
    }
    catch (fs::filesystem_error const& e) {
        logger.Write(LOG_ERROR, "[%s] Restore installerdata error %s", __FUNCTION__, e.what());
    }

    try {
        std::string eaac_path = GetEAACLauncherPath();
        if (!eaac_path.empty()) {
            std::string bak = eaac_path + ".backup";
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