#include <files_manager/files_manager.h>

namespace LE {
    FilesManager::FilesManager() {
        tool_root.clear();
        game_root.clear();
        data_root.clear();
        mods_root.clear();

        char fullModPath[MAX_PATH];
        if (GetModuleFileNameA(GetModuleHandle(NULL), fullModPath, MAX_PATH))
        {
            tool_root = fs::path(fullModPath).parent_path();
        }
    }
    FilesManager::~FilesManager() {
    
    }

#pragma region EAAC
    void FilesManager::BackupAnticheat() {
        LOG_FUNC_START();

        try {
            fs::path anticheat_path = GetAnticheatLauncherPath();
            if (anticheat_path.empty()) return;

            if (!fs::exists(anticheat_path)) {
                LOG_ERROR(std::format("Anticheat Launcher not found ({})", ToUTF8String(anticheat_path).c_str()));
                return;
            }

            // Check if Anticheat Launcher is bigger than 1MB to enusre it's not a fake one
            if (fs::file_size(anticheat_path) > 0x100000) {
                fs::path anticheat_path_bak = anticheat_path.parent_path() / (anticheat_path.filename().string() + ".backup");
                if (fs::exists(anticheat_path_bak))    fs::remove(anticheat_path_bak);
                fs::copy(anticheat_path, anticheat_path_bak);
            }
        }
        catch (fs::filesystem_error const& e) {
            LOG_ERROR(std::format("[{}] error {}", __FUNCTION__, e.what()));
        }

        LOG_FUNC_END();
    }

    void FilesManager::RestoreAnticheat() {
        LOG_FUNC_START();

        try {
            fs::path anticheat_path = GetAnticheatLauncherPath();
            if (anticheat_path.empty()) return;

            fs::path anticheat_path_bak = anticheat_path.parent_path() / (anticheat_path.filename().string() + ".backup");

            // Check if Backup Anticheat Launcher is bigger than 1MB to enusre it's not a fake one
            if (
                fs::exists(anticheat_path_bak) && 
                fs::file_size(anticheat_path_bak) > 0x100000
            ) {
                if (fs::exists(anticheat_path))    fs::remove(anticheat_path);
                fs::copy(anticheat_path_bak, anticheat_path);
            }
        }
        catch (fs::filesystem_error const& e) {
            LOG_ERROR(std::format("[{}] error {}", __FUNCTION__, e.what()));
        }
        LOG_FUNC_END();
    }

    void FilesManager::InstallFakeAnticheat() {
        fs::path game_loc = GetGameProcessFullPath();
        if (!fs::exists(game_loc)) return;

        RestoreAnticheat();
        BackupAnticheat();

        LOG_FUNC_START();

        std::filesystem::path fake_eaac_path = GetFakeAnticheatLauncherPath();
        if (fake_eaac_path.empty() || !fs::exists(fake_eaac_path)) {
            LOG_FATAL(std::format("Fake Anticheat Launcher not found {}", ToUTF8String(fake_eaac_path).c_str()));
            return;
        }

        try {
            fs::path anticheat_path = GetAnticheatLauncherPath();
            if (fs::exists(anticheat_path)) {
                if (fs::exists(anticheat_path))    fs::remove(anticheat_path);
                fs::copy(fake_eaac_path, anticheat_path);
            }
        }
        catch (fs::filesystem_error const& e) {
            LOG_ERROR(std::format("[{}] error {}", __FUNCTION__, e.what()));
        }

        LOG_FUNC_END();
    }

    void FilesManager::UnInstallFakeAnticheat() {
        LOG_FUNC_START();

        fs::path anticheat_path = GetAnticheatLauncherPath();
        if (anticheat_path.empty() || !fs::exists(anticheat_path)) return;

        while (true)
        {
            RestoreAnticheat();
            if (fs::file_size(anticheat_path) > 0x100000) break;
            Sleep(250);
        }

        fs::path anticheat_path_bak = anticheat_path.parent_path() / (anticheat_path.filename().string() + ".backup");
        if (fs::exists(anticheat_path_bak))     fs::remove(anticheat_path_bak);

        LOG_FUNC_END();
    }

#pragma endregion EAAC

    void FilesManager::CreateDirectories() {
        // Creates C:\FC <YEAR> Live Editor
        auto data_path = GetLEDataDirectory();
        SafeCreateDirectories(data_path);
        SafeCreateDirectories(data_path / "extensions");
        SafeCreateDirectories(data_path / "keys");
        SafeCreateDirectories(data_path / "lua");
        SafeCreateDirectories(data_path / "lua" / "autorun");

        // Creates C:\FC <YEAR> Live Editor\mods
        auto mods_path = GetLEModsDirectory();
        SafeCreateDirectories(mods_path);
        SafeCreateDirectories(mods_path / "legacy");
    }

    fs::path FilesManager::GetRoot() {
        return tool_root;
    }

    fs::path FilesManager::GetGameDirectory() {
        if (!game_root.empty()) return game_root;

        fs::path result = GetCustomGameInstallDirFromReg();
        if (
            result.empty() ||
            !fs::exists(result) ||
            !fs::is_directory(result)
        ) {
            result = GetGameInstallDirFromReg();
        }

        if (result.empty()) {
            LOG_WARN("Can't find game directory in registry");
        }

        if (fs::exists(result))
            game_root = result;

        return result;
    }

    fs::path FilesManager::GetLEDataDirectory() {
        if (!data_root.empty())   return data_root;

        fs::path result = GetLEDataDirFromReg();
        if (!fs::exists(result) || !fs::is_directory(result))    result.clear();

        if (result.empty()) {
            char* system_drive{ nullptr };
            size_t count{ 0 };
            _dupenv_s(&system_drive, &count, "SystemDrive");

            std::string sys_drive_letter = std::string(system_drive) + "\\";

            result = sys_drive_letter;
            result /= "FC " + std::to_string(EAFC_EDITION) + " Live Editor";

            free(system_drive);

            SetLEDataDir(result);
        }

        if (fs::exists(result))
            data_root = result;

        return result;
    }

    fs::path FilesManager::GetLEModsDirectory() {
        if (!mods_root.empty()) return mods_root;

        fs::path result = GetLEModsDirFromReg();
        if (!fs::exists(result) || !fs::is_directory(result))    result.clear();

        if (result.empty()) {
            result = GetLEDataDirectory() / "mods";
            SetLEModsDir(result);
        }

        if (fs::exists(result))
            mods_root = result;

        return result;
    }

    fs::path FilesManager::GetKeysDirectory() {
        return GetLEDataDirectory() / "keys";
    }

    fs::path FilesManager::GetAnticheatLauncherPath() {
        fs::path result = GetGameDirectory();
        if (result.empty()) return result;

        return result / "EAAntiCheat.GameServiceLauncher.exe";
    }

    fs::path FilesManager::GetFakeAnticheatLauncherPath() {
        return  tool_root / "FakeEAACLauncher" / "EAAntiCheat.GameServiceLauncher.exe";
    }

    fs::path FilesManager::GetLangPath() {
        return tool_root / "loc" / "eng_us" / "localize.json";
    }

    fs::path FilesManager::GetConfigPath() {
        return GetLEDataDirectory() / "le_config.json";
    }

    fs::path FilesManager::GetLocaleKeyPath() {
        return GetKeysDirectory() / "localeini.key";
    }

    // HKEY_LOCAL_MACHINE\SOFTWARE\EA Sports\EA SPORTS FC <YEAR>\Install Dir
    fs::path FilesManager::GetGameInstallDirFromReg() {
        fs::path result;

        HKEY hKey;
        std::string key = std::format("SOFTWARE\\EA Sports\\EA SPORTS FC {}",EAFC_EDITION);
        LSTATUS open_status = RegOpenKey(HKEY_LOCAL_MACHINE, key.c_str(), &hKey);
        if (open_status != ERROR_SUCCESS) {
            LOG_ERROR(std::format("[{}] RegOpenKey failed. Status: {}", __FUNCTION__, open_status));
            RegCloseKey(hKey);

            result.clear();
            return result;
        }

        char value_buf[1024];
        DWORD value_length = sizeof(value_buf);
        DWORD dwType = REG_SZ;
        LSTATUS query_status = RegQueryValueEx(hKey, "Install Dir", NULL, &dwType, reinterpret_cast<LPBYTE>(value_buf), &value_length);
        RegCloseKey(hKey);

        if (query_status != ERROR_SUCCESS) {
            LOG_ERROR(std::format("[{}] RegQueryValueEx failed. Status: {}", __FUNCTION__, query_status));
            result.clear();
            return result;
        }
        result = fs::path(value_buf);

        return result;
    }

    // HKEY_LOCAL_MACHINE\SOFTWARE\Live Editor\FC <YEAR>\Game Dir
    fs::path FilesManager::GetCustomGameInstallDirFromReg() {
        return GetLERegPathKey("Game Dir");
    }

    // HKEY_LOCAL_MACHINE\SOFTWARE\Live Editor\FC <YEAR>\Data Dir
    fs::path FilesManager::GetLEDataDirFromReg() {
        return GetLERegPathKey("Data Dir");
    }

    // HKEY_LOCAL_MACHINE\SOFTWARE\Live Editor\FC <YEAR>\Mods Dir
    fs::path FilesManager::GetLEModsDirFromReg() {
        return GetLERegPathKey("Mods Dir");
    }

    bool FilesManager::SetCustomGameDir(fs::path _dir) {
        game_root = _dir;
        return SetLERegPathKey(L"Game Dir", _dir);
    }

    bool FilesManager::SetLEDataDir(fs::path _dir) {
        data_root = _dir;
        return SetLERegPathKey(L"Data Dir", _dir);
    }

    bool FilesManager::SetLEModsDir(fs::path _dir) {
        mods_root = _dir;
        return SetLERegPathKey(L"Mods Dir", _dir);
    }

    fs::path FilesManager::GetGameProcessFullPath() {
        LE::Config* le_config = LE::Config::GetInstance();
        std::string proc_name = le_config->IsTrial() ? le_config->GetProcNameTrial() : le_config->GetProcName();

        return GetGameDirectory() / proc_name;
    }

    const char* FilesManager::GetImGuiIni() {
        return ToUTF8String(GetLEDataDirectory() / "le_launcher_imgui.ini").c_str();
    }

    void FilesManager::DetectFIFAModManager() {
        fs::path game_loc = GetGameDirectory();
        if (game_loc.empty() || !fs::exists(game_loc)) return;

        std::filesystem::path FIFAModDataDir = tool_root / "FIFAModData";
        if (std::filesystem::exists(FIFAModDataDir)) {
            LOG_INFO("Found FIFAModData in game directory");
        }
    }

    void FilesManager::SetupLogger() {
        const fs::path logPath = tool_root / "Logs";
        if (!fs::is_directory(logPath) || !fs::exists(logPath)) {
            fs::create_directory(logPath);
        }

        SYSTEMTIME currTimeLog;
        GetLocalTime(&currTimeLog);
        std::ostringstream ssLogFile;
        ssLogFile << "live_editor_launcher_" <<
            std::setw(2) << std::setfill('0') << currTimeLog.wDay << "-" <<
            std::setw(2) << std::setfill('0') << currTimeLog.wMonth << "-" <<
            std::setw(4) << std::setfill('0') << currTimeLog.wYear << ".log";
        const fs::path logFile = logPath / ssLogFile.str();
        defaultLogger.SetFile(logFile);
    }

    void FilesManager::SetupConfig() {
        LE::Config::GetInstance()->Init(GetConfigPath());
    }

    void FilesManager::DetectAnadius() {
        fs::path game_loc = GetGameDirectory();
        if (game_loc.empty() || !fs::exists(game_loc)) return;

        std::filesystem::path Anadiuscfg = game_loc / "anadius.cfg";
        if (std::filesystem::exists(Anadiuscfg)) {
            LOG_INFO("anadius.cfg");
        }
    }

    std::string FilesManager::ToUTF8String(const std::filesystem::path& value) {
        auto result = value.u8string();

        return { result.begin(), result.end() };
    }

    bool FilesManager::SetLERegPathKey(const wchar_t* value_name, fs::path _dir) {
        std::wstring new_dir_path = _dir.wstring();

        HKEY hKey;
        std::string key = std::format("SOFTWARE\\{}\\FC {}", TOOL_NAME, EAFC_EDITION);
        if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
            LOG_WARN(std::format("[{}] RegCreateKeyExA failed", __FUNCTION__));
            RegCloseKey(hKey);
            return false;
        }

        DWORD cbData = static_cast<DWORD>((new_dir_path.size() + static_cast<size_t>(1)) * sizeof(wchar_t));
        LSTATUS lSetStatus = RegSetValueExW(hKey, value_name, 0, REG_SZ, (LPBYTE)new_dir_path.c_str(), cbData);
        RegCloseKey(hKey);

        if (lSetStatus != ERROR_SUCCESS)
        {
            LOG_WARN(std::format("[{}] RegSetValueExW failed {}", __FUNCTION__, lSetStatus));
            return false;
        }

        return true;
    }

    fs::path FilesManager::GetLERegPathKey(const char* value_name) {
        fs::path result;

        HKEY hKey;
        std::string key = std::format("SOFTWARE\\{}\\FC {}", TOOL_NAME, EAFC_EDITION);
        LSTATUS open_status = RegOpenKey(HKEY_LOCAL_MACHINE, key.c_str(), &hKey);
        if (open_status != ERROR_SUCCESS) {
            // LOG_ERROR(std::format("[{}] RegOpenKey failed. Status: {}", __FUNCTION__, open_status));
            RegCloseKey(hKey);

            result.clear();
            return result;
        }

        char value_buf[1024];
        DWORD value_length = sizeof(value_buf);
        DWORD dwType = REG_SZ;
        LSTATUS query_status = RegQueryValueEx(hKey, value_name, NULL, &dwType, reinterpret_cast<LPBYTE>(value_buf), &value_length);
        RegCloseKey(hKey);

        if (query_status != ERROR_SUCCESS) {
            // LOG_ERROR(std::format("[{}] RegQueryValueEx failed. Status: {}", __FUNCTION__, query_status));
            result.clear();
            return result;
        }
        result = fs::path(value_buf);

        return result;
    }

    void FilesManager::SafeCreateDirectories(fs::path _dir) {
        if (fs::exists(_dir)) return;

        try {
            fs::create_directories(_dir);
        }
        catch (fs::filesystem_error const& e) {
            LOG_ERROR(std::format("Create Directory failed {} ({})", e.what(), ToUTF8String(_dir).c_str()));
            return;
        }

        if (!fs::exists(_dir)) {
            LOG_ERROR(std::format("Create Directory failed ({})", ToUTF8String(_dir).c_str()));
        }
    }

#pragma region UTF8Directories
    std::string FilesManager::GetGameDirectoryU8() {
        return ToUTF8String(GetGameDirectory());
    }

    std::string FilesManager::GetLEDataDirectoryU8() {
        return ToUTF8String(GetLEDataDirectory());
    }

    std::string FilesManager::GetLEModsDirectoryU8() {
        return ToUTF8String(GetLEModsDirectory());
    }
#pragma endregion UTF8Directories

    FilesManager* FilesManager::GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
            pinstance_ = new FilesManager();

        return pinstance_;
    }
}

LE::FilesManager* LE::FilesManager::pinstance_{ nullptr };
std::mutex LE::FilesManager::mutex_;