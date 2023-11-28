#include <core.h>

Core::Core()
{
}

Core::~Core()
{
}

bool Core::Init()
{
    hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, "LELauncherMutex");
    if (!hMutex)
    {
        hMutex = CreateMutex(0, 0, "LELauncherMutex");
    }
    else
    {
        MessageBox(NULL, "Live Editor Launcher is already open", "ERROR", MB_ICONERROR);
        return false;
    }

    ctx.Update(GetModuleHandle(NULL));
    std::filesystem::path le_dir = ctx.GetFolder();
    std::string app_data("AppData\\Local\\Temp");
    if (ToUTF8String(le_dir).find(app_data) != std::string::npos) {
        MessageBox(NULL, "Archive not extracted\n\nUnpack live editor with winrar or alternative software if you want to use it", "Not extracted", MB_ICONERROR);
        return false;
    }

    SetupLogger();
    if (!InitDirectories()) {
        MessageBox(NULL, "Init Directories has failed. More info in log file", "ERROR", MB_ICONERROR);
        return false;
    }

    std::filesystem::path game_install_dir = GetGameInstallDir();

    logger.Write(LOG_INFO, "[%s] %s %s", __FUNCTION__, TOOL_NAME, TOOL_VERSION);
    logger.Write(LOG_INFO, "[%s] Game Install Dir: %s", __FUNCTION__, ToUTF8String(game_install_dir).c_str());
    logger.Write(LOG_INFO, "[%s] Live Editor Dir: %s", __FUNCTION__, ToUTF8String(le_dir).c_str());

    std::string procname = "FC" + std::to_string(FIFA_EDITION) + ".exe";
    std::filesystem::path proc_full_path = game_install_dir / procname;
    if (!std::filesystem::exists(proc_full_path)) {
        std::string msg = "Can't find " + procname + " in:\n" + ToUTF8String(game_install_dir);
        logger.Write(LOG_FATAL, "[%s] %s ", __FUNCTION__, msg.c_str());
        MessageBox(NULL, msg.c_str(), "ERROR", MB_ICONERROR);
    }
    ReadGameBuildInfo();

    RestoreOrgGameFiles();
    BackupOrgGameFiles();
    CopyFakeEAAC();
    SetupLocalize();

    g_options_ids.SetFile(GetLEDataPath());
    g_options_ids.LoadJson();

    SetLEPathRegVal(le_dir.wstring());
    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);

    return true;
}

void Core::onExit() {
    std::filesystem::path eaac_path = GetEAACLauncherPath();
    if (eaac_path.empty())      return;

    fs::path bak = GetBAKPathFor(eaac_path);
    if (!fs::exists(bak))       return;

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
            Sleep(10);
        }
        CloseHandle(snapshot);
        Sleep(100);
    } while (pid > 0);

    logger.Write(LOG_INFO, "[%s] Trying to restore", __FUNCTION__);
    RestoreOrgGameFiles();
    ReleaseMutex(hMutex);
}

void Core::DetectFIFAModManager() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    // Check if FIFAModData exists in game dir
    std::filesystem::path FIFAModDataDir = GetGameInstallDir() / "FIFAModData";
    if (std::filesystem::exists(FIFAModDataDir)) {
        logger.Write(LOG_INFO, "[%s] Found %s", __FUNCTION__, ToUTF8String(FIFAModDataDir).c_str());
    }

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

const char* Core::GetToolVer() {
    return TOOL_VERSION;
}

std::string Core::GetGameVer() {
    std::string result = "0.0.0.0";

    fs::path fpath = GetGameInstallDir() / "__Installer" / "installerdata.xml";
    if (!fs::exists(fpath)) {
        logger.Write(LOG_ERROR, "[%s] Can't find %s", __FUNCTION__, ToUTF8String(fpath).c_str());
        return result;
    }

    FILE* f = _wfopen(fpath.wstring().c_str(), L"rb");
    if (!f) {
        logger.Write(LOG_ERROR, "[%s] Can't open %s", __FUNCTION__, ToUTF8String(fpath).c_str());
        return result;
    }

    char* fbuf = nullptr;

    fseek(f, 0, SEEK_END);
    __int64 fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize <= 0) {
        logger.Write(LOG_ERROR, "[%s] File is empty(?) %s", __FUNCTION__, ToUTF8String(fpath).c_str());
        return result;
    }

    fbuf = new char[fsize];
    fread(fbuf, fsize, 1, f);
    fclose(f);

    tinyxml2::XMLDocument xmlDoc;
    xmlDoc.Parse(fbuf, fsize);
    tinyxml2::XMLElement* DiPManifest = xmlDoc.FirstChildElement("DiPManifest");
    if (DiPManifest) {
        tinyxml2::XMLElement* buildMetaData = DiPManifest->FirstChildElement("buildMetaData");
        if (buildMetaData) {
            tinyxml2::XMLElement* gameVersion = buildMetaData->FirstChildElement("gameVersion");
            if (gameVersion) {
                result = std::string(gameVersion->Attribute("version"));
            }
        }

    }

    delete[] fbuf;
    return result;
}

std::string Core::GetTU() {
    std::string game_ver = GetGameVer();
    logger.Write(LOG_INFO, "[%s] gameVersion %s", __FUNCTION__, game_ver.c_str());

    if (GAME_VERSION_TU_MAP.contains(game_ver)) {
        return GAME_VERSION_TU_MAP.at(game_ver);
    }

    auto ver_splitted = splitStr(game_ver, ".");
    if (ver_splitted.size() != 4) {
        return "Invalid";
    }

    int minor = std::stoi(ver_splitted[ver_splitted.size() - 1]);
    int major = std::stoi(ver_splitted[ver_splitted.size() - 2]);

    // No info about exact version for TU1, do compare with Vanilla and TU2
    if (
        (major == 83 && minor > 56686) ||
        (major == 84 && minor < 7390)
    ) {
        // Is between Vanilla & TU2, so must be TU1
        return "TU1";
    }

    if (major >= LATEST_MAJOR_GAME_VER && minor >= LATEST_MINOR_GAME_VER) {
        return "TU" + std::to_string(LATEST_TU+1) + " or newer";
    }

    return game_ver;
}

fs::path Core::GetGameInstallDir() {
    if (!g_Config.directories_values.game_loc.empty()) {
        return g_Config.directories_values.game_loc;
    }

    // Computer\HKEY_LOCAL_MACHINE\SOFTWARE\EA Sports\EA SPORTS FC 24
    DWORD dwType = REG_SZ;
    HKEY hKey = 0;
    std::string subkey = std::string("SOFTWARE\\EA Sports\\EA SPORTS FC ") + std::to_string(FIFA_EDITION);

    LSTATUS open_status = RegOpenKey(HKEY_LOCAL_MACHINE, subkey.c_str(), &hKey);
    if (open_status != ERROR_SUCCESS) {
        logger.Write(LOG_ERROR, "[%s] RegOpenKey failed %d", __FUNCTION__, open_status);
    }
    
    const char* val_name = "Install Dir";

    char value_buf[1024];
    DWORD value_length = sizeof(value_buf);
    LSTATUS query_status = RegQueryValueEx(hKey, val_name, NULL, &dwType, reinterpret_cast<LPBYTE>(value_buf), &value_length);
    RegCloseKey(hKey);

    if (query_status != ERROR_SUCCESS) {
        logger.Write(LOG_ERROR, "[%s] RegQueryValueEx failed %d", __FUNCTION__, query_status);
        return fs::path("");
    }


    return fs::path(value_buf);
}

void Core::RunGame() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    std::string proc_name = g_Config.launch_values.game_proc_name;
    std::wstring params = L"";

    if (g_Config.launch_values.is_trial) {
        proc_name = g_Config.launch_values.game_proc_name_trial;
        params = L"-trial";
    }

    std::filesystem::path game_full_path = GetGameInstallDir() / proc_name;

    if (fs::exists(game_full_path)) {
        auto sFullPath = ToUTF8String(game_full_path);

        logger.Write(LOG_INFO, "[%s] game_full_path: %s", __FUNCTION__, sFullPath.c_str());

        SHELLEXECUTEINFOW ShExecInfo;
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ShExecInfo.fMask = NULL;
        ShExecInfo.hwnd = NULL;
        ShExecInfo.lpVerb = L"runas";
        ShExecInfo.lpFile = wcsdup(game_full_path.wstring().c_str());
        ShExecInfo.lpParameters = wcsdup(params.c_str());
        ShExecInfo.lpDirectory = NULL;
        ShExecInfo.nShow = SW_SHOWDEFAULT;
        ShExecInfo.hInstApp = NULL;

        bool result = ShellExecuteExW(&ShExecInfo);
        if (!result) {
            DWORD err = GetLastError();
            logger.Write(LOG_INFO, "[%s] Done %d, err %d %s", __FUNCTION__, result, err, std::system_category().message(err).c_str());
        }
        else {
            logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
        }
    }
    else {
        std::wstring msg = L"Can't Find File:\n" + game_full_path.wstring();

        MessageBoxW(NULL, msg.c_str(), L"ERROR", MB_ICONERROR);
    }
}

bool Core::SafeCreateDirectories(const std::filesystem::path d) {
    try {
        if (fs::exists(d))   return false;
        if (!fs::create_directories(d)) {
            logger.Write(LOG_ERROR, "[%s] Create Directory failed %s", __FUNCTION__, ToUTF8String(d).c_str());
            return false;
        }
    }
    catch (fs::filesystem_error const& e) {
        logger.Write(LOG_ERROR, "[%s] Create Directory failed %s (%s)", __FUNCTION__, e.what(), ToUTF8String(d).c_str());
        return false;
    }
    return true;
}

void Core::SetupLogger() {
    const std::filesystem::path logPath = ctx.GetFolder() / "Logs";
    std::string msg = "Failed to create Logs directory:\n" + ToUTF8String(logPath) + "\nError: ";
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

fs::path Core::GetLEDataPathRegVal() {
    fs::path result;

    HKEY hKey;
    std::string key = std::string("SOFTWARE\\Live Editor\\FC ") + std::to_string(FIFA_EDITION) + "\\Data Dir";

    logger.Write(LOG_INFO, "[%s] %s", __FUNCTION__, key.c_str());
    LSTATUS lOpenStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &hKey);
    if (lOpenStatus != ERROR_SUCCESS) {
        logger.Write(LOG_WARN, "[%s] RegOpenKeyEx %s failed %d", __FUNCTION__, key.c_str(), lOpenStatus);
        RegCloseKey(hKey);
        return result;
    }

    DWORD dwType = REG_SZ;
    wchar_t value_buf[1024];
    DWORD value_length = sizeof(value_buf);
    LSTATUS query_status = RegQueryValueExW(hKey, L"Data Dir", NULL, &dwType, reinterpret_cast<LPBYTE>(value_buf), &value_length);
    RegCloseKey(hKey);
    if (query_status != ERROR_SUCCESS) {
        logger.Write(LOG_WARN, "[%s] RegQueryValueEx failed %d", __FUNCTION__, query_status);
        return fs::path("");
    }

    return fs::path(value_buf);
}

bool Core::SetLEPathRegVal(std::wstring data) {
    HKEY hKey;
    std::string key = std::string("SOFTWARE\\Live Editor\\FC ") + std::to_string(FIFA_EDITION) + "\\Dir";
    if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        logger.Write(LOG_WARN, "[%s] RegCreateKeyExA failed ", __FUNCTION__);
        return false;
    }
    else {
        DWORD cbData = static_cast<DWORD>((data.size() + static_cast<size_t>(1)) * sizeof(wchar_t));
        LSTATUS lSetStatus = RegSetValueExW(hKey, L"Dir", 0, REG_SZ, (LPBYTE)data.c_str(), cbData);
        RegCloseKey(hKey);

        if (lSetStatus != ERROR_SUCCESS)
        {
            logger.Write(LOG_WARN, "[%s] RegSetValueExW failed %d", __FUNCTION__, lSetStatus);
            return false;
        }
    }

    return true;
}

bool Core::SetLEDataPathRegVal(std::wstring data)
{
    HKEY hKey;
    std::string key = std::string("SOFTWARE\\Live Editor\\FC ") + std::to_string(FIFA_EDITION) + "\\Data Dir";
    if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        logger.Write(LOG_WARN, "[%s] RegCreateKeyExA failed ", __FUNCTION__);
        return false;
    }
    else {
        DWORD cbData = static_cast<DWORD>((data.size() + static_cast<size_t>(1)) * sizeof(wchar_t));
        LSTATUS lSetStatus = RegSetValueExW(hKey, L"Data Dir", 0, REG_SZ, (LPBYTE)data.c_str(), cbData);
        RegCloseKey(hKey);

        if (lSetStatus != ERROR_SUCCESS)
        {
            logger.Write(LOG_WARN, "[%s] RegSetValueExW failed %d", __FUNCTION__, lSetStatus);
            return false;
        }
    }

    return true;
}

fs::path Core::GetLEDataPath() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    std::filesystem::path result = GetLEDataPathRegVal();

    if (!result.empty()) {
        logger.Write(LOG_INFO, "[%s] Found Reg Val: %s", __FUNCTION__, ToUTF8String(result).c_str());
        return result;
    }
    else {
        std::filesystem::path result(std::string(std::getenv("SystemDrive")) + "\\");
        result /= "FC " + std::to_string(FIFA_EDITION) + " Live Editor";

        if (!SetLEDataPathRegVal(result.wstring())) {
            logger.Write(LOG_WARN, "[%s] SetLEDataPathRegVal failed", __FUNCTION__);
        }

        logger.Write(LOG_INFO, "[%s] Default: %s", __FUNCTION__, ToUTF8String(result).c_str());
        return result;
    }
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


void Core::SetupLocalize() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);
    localize.SetLangPath(ctx.GetFolder());
    localize.LoadLangTrans("EN");

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

void Core::ReadGameBuildInfo() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);
    std::filesystem::path build_info_dll_path = GetGameInstallDir() / "Engine.BuildInfo.dll";

    if (fs::exists(build_info_dll_path)) {
        logger.Write(LOG_INFO, "[%s] file path: %s", __FUNCTION__, ToUTF8String(build_info_dll_path).c_str());
        game_build_info = BuildInfo::GetInstance(build_info_dll_path);

        if (game_build_info) {
            logger.Write(LOG_INFO, "----------------------[Frostbite BuildInfo]----------------------");
            logger.Write(LOG_INFO, "BranchName:                         %s", game_build_info->getBranchName());
            logger.Write(LOG_INFO, "LicenseeId:                         %s", game_build_info->getLicenseeId());
            logger.Write(LOG_INFO, "StudioName:                         %s", game_build_info->getStudioName());
            logger.Write(LOG_INFO, "Changelist:                         %d", game_build_info->getChangelist());
            logger.Write(LOG_INFO, "SourceChangeList:                   %d", game_build_info->getSourceChangelist());
            logger.Write(LOG_INFO, "FrostbiteChangelist:                %d", game_build_info->getFrostbiteChangelist());
            logger.Write(LOG_INFO, "FrostbiteRelease:                   %s", game_build_info->getFrostbiteRelease());
            logger.Write(LOG_INFO, "IsAutoBuild:                        %s", game_build_info->getIsAutoBuild() ? "true" : "false");
            logger.Write(LOG_INFO, "Username:                           %s", game_build_info->getUsername());
            logger.Write(LOG_INFO, "BuildTime:                          %s", game_build_info->getBuildTime());
            logger.Write(LOG_INFO, "BuildDate:                          %s", game_build_info->getBuildDate());
            logger.Write(LOG_INFO, "BuildIsoDate:                       %s", game_build_info->getBuildIsoDate());
            logger.Write(LOG_INFO, "-----------------------------------------------------------------");
        }
    }

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

bool Core::InitDirectories() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    bool config_save_required = false;

    std::filesystem::path live_editor_path = GetLEDataPath(); 
    SafeCreateDirectories(live_editor_path);

    g_Config.Setup(live_editor_path);
    g_Config.Load();

    std::filesystem::path live_editor_data_path = live_editor_path / "data";
    SafeCreateDirectories(live_editor_data_path);

    std::filesystem::path live_editor_career_data_path = live_editor_path / "career_data";
    SafeCreateDirectories(live_editor_career_data_path);

    std::filesystem::path live_editor_lua_libs = live_editor_path / "lua";
    SafeCreateDirectories(live_editor_lua_libs);

    std::filesystem::path live_editor_mods_path = live_editor_path / "mods";

    if (g_Config.directories_values.mods_root.empty()) {
        g_Config.directories_values.mods_root = live_editor_mods_path;
        config_save_required |= true;
    }
    else {
        live_editor_mods_path = g_Config.directories_values.mods_root;
    }
    SafeCreateDirectories(live_editor_mods_path);

    std::filesystem::path live_editor_lua_autorun = live_editor_mods_path / "lua_autorun";
    SafeCreateDirectories(live_editor_lua_autorun);

    if (g_Config.directories_values.filters_storage.empty()) {
        g_Config.directories_values.filters_storage = live_editor_data_path / "filters";
        config_save_required |= true;
    }

    if (g_Config.directories_values.legacyfolder_export.empty()) {
        g_Config.directories_values.legacyfolder_export = live_editor_mods_path;
        config_save_required |= true;
    }

    // copy files
    const std::filesystem::path dest_legacystructure = live_editor_data_path / "legacy_structure.txt";
    const std::filesystem::path dest_dbmeta = live_editor_data_path / "db_meta.xml";
    const std::filesystem::path dest_idmap = live_editor_data_path / "id_map.json";
    const std::filesystem::path cur_data_path = ctx.GetFolder() / "data";
    if (std::filesystem::exists(cur_data_path)) {
        if (!std::filesystem::exists(dest_legacystructure)) {
            logger.Write(LOG_INFO, "[%s] Copy legacy_structure.txt to %s", __FUNCTION__, ToUTF8String(dest_legacystructure).c_str());
            std::filesystem::copy_file(cur_data_path / "legacy_structure.txt", dest_legacystructure);
        }

        if (!std::filesystem::exists(dest_dbmeta)) {
            logger.Write(LOG_INFO, "[%s] Copy db_meta.xml to %s", __FUNCTION__, ToUTF8String(dest_dbmeta).c_str());
            std::filesystem::copy_file(cur_data_path / "db_meta.xml", dest_dbmeta);
        }

        if (!std::filesystem::exists(dest_idmap)) {
            logger.Write(LOG_INFO, "[%s] Copy id_map.json to %s", __FUNCTION__, ToUTF8String(dest_idmap).c_str());
            std::filesystem::copy_file(cur_data_path / "id_map.json", dest_idmap);
        }
    }

    const std::filesystem::path cur_filters_path = ctx.GetFolder() / "filters";
    if (std::filesystem::exists(cur_filters_path)) {
        const std::filesystem::path cur_players_filters_path = cur_filters_path / "players";
        const std::filesystem::path cur_teams_filters_path = cur_filters_path / "teams";

        const std::filesystem::path dest_players_filters = g_Config.directories_values.filters_storage / "players";
        const std::filesystem::path dest_teams_filters = g_Config.directories_values.filters_storage / "teams";

        SafeCreateDirectories(dest_players_filters);
        SafeCreateDirectories(dest_teams_filters);

        if (std::filesystem::exists(cur_players_filters_path)) {
            for (const auto& entry : std::filesystem::directory_iterator(cur_players_filters_path))
            {
                if (entry.is_regular_file())
                {
                    std::filesystem::path new_loc = dest_players_filters / entry.path().filename();
                    if (!std::filesystem::exists(new_loc)) {
                        std::filesystem::copy_file(entry.path(), new_loc);
                    }
                }
            }
        }
        
        if (std::filesystem::exists(cur_teams_filters_path)) {
            for (const auto& entry : std::filesystem::directory_iterator(cur_teams_filters_path))
            {
                if (entry.is_regular_file())
                {
                    std::filesystem::path new_loc = dest_teams_filters / entry.path().filename();
                    if (!std::filesystem::exists(new_loc)) {
                        std::filesystem::copy_file(entry.path(), new_loc);
                    }
                }
            }
        }
    }

    CreateLegacyFilesStructure(dest_legacystructure, live_editor_mods_path);

    if (config_save_required) {
        g_Config.Save();
    }

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
    return true;
}

void Core::CreateLegacyFilesStructure(std::filesystem::path folders_list, std::filesystem::path mods_dir) {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);
    
    std::filesystem::path legacy_root = mods_dir / "root";
    SafeCreateDirectories(legacy_root);
    legacy_root /= "Legacy";
    SafeCreateDirectories(legacy_root);

    std::ifstream folders_to_create(folders_list);
    std::string legacy_folder;
    std::filesystem::path full_path = "";
    int created_folders_count = 0;
    while (std::getline(folders_to_create, legacy_folder)) {
        if (SafeCreateDirectories(legacy_root / legacy_folder)) {
            created_folders_count++;
        }
    }

    if (created_folders_count > 0) {
        logger.Write(LOG_INFO, "[%s] Created %d legacy folders", __FUNCTION__, created_folders_count);
    }

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

Core g_Core;