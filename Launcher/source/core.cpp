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

    LOG_INFO(std::format("{} {}", TOOL_NAME, TOOL_VERSION));
    LOG_INFO(std::format("Game Install Dir: {}", ToUTF8String(game_install_dir)));
    LOG_INFO(std::format("Live Editor Dir: {}", ToUTF8String(le_dir)));

    std::string procname = "FC" + std::to_string(FIFA_EDITION) + ".exe";
    std::filesystem::path proc_full_path = game_install_dir / procname;
    if (!std::filesystem::exists(proc_full_path)) {
        std::string msg = std::format("Can't find {} in :\n{}", procname, ToUTF8String(game_install_dir));
        LOG_FATAL(msg);
    }
    ReadGameBuildInfo();

    RestoreOrgGameFiles();
    BackupOrgGameFiles();
    CopyFakeEAAC();
    SetupLocalize();

    g_options_ids.SetFile(GetLEDataPath());
    g_options_ids.LoadJson();

    SetLEPathRegVal(le_dir.wstring());
    LOG_INFO(std::format("[{}] Done", __FUNCTION__));

    return true;
}

void Core::onExit() {
    std::filesystem::path eaac_path = GetEAACLauncherPath();
    if (eaac_path.empty())      return;

    fs::path bak = GetBAKPathFor(eaac_path);
    if (!fs::exists(bak))       return;

    LOG_INFO("Waiting for EAAntiCheat.GameServiceLauncher.exe");

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

    LOG_INFO("Trying to restore");
    RestoreOrgGameFiles();
    ReleaseMutex(hMutex);
}

void Core::DetectFIFAModManager() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));

    // Check if FIFAModData exists in game dir
    std::filesystem::path FIFAModDataDir = GetGameInstallDir() / "FIFAModData";
    if (std::filesystem::exists(FIFAModDataDir)) {
        LOG_INFO(std::format("[{}] Found {}", __FUNCTION__, ToUTF8String(FIFAModDataDir)));
    }

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
}

const char* Core::GetToolVer() {
    return TOOL_VERSION;
}

std::string Core::GetGameVer() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));
    std::string result = "0.0.0.0";

    fs::path fpath = GetGameInstallDir() / "__Installer" / "installerdata.xml";
    if (!fs::exists(fpath)) {
        LOG_ERROR(std::format("[{}] Can't find {}", __FUNCTION__, ToUTF8String(fpath).c_str()));
        return result;
    }

    FILE* f = _wfopen(fpath.wstring().c_str(), L"rb");
    if (!f) {
        LOG_ERROR(std::format("[{}] Can't open {}", __FUNCTION__, ToUTF8String(fpath).c_str()));
        return result;
    }

    fseek(f, 0, SEEK_END);
    __int64 fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize <= 0) {
        LOG_ERROR(std::format("[{}] File is empty(?) {}", __FUNCTION__, ToUTF8String(fpath).c_str()));
        return result;
    }

    char* fbuf = new char[fsize];
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
    LOG_INFO(std::format("[{}] gameVersion {}", __FUNCTION__, game_ver));

    if (GAME_VERSION_TU_MAP.contains(game_ver)) {
        return GAME_VERSION_TU_MAP.at(game_ver);
    }

    auto ver_splitted = splitStr(game_ver, ".");
    if (ver_splitted.size() != 4) {
        return "Invalid";
    }

    int minor = std::stoi(ver_splitted[ver_splitted.size() - 1]);
    int major = std::stoi(ver_splitted[ver_splitted.size() - 2]);

    if (
        major > LATEST_MAJOR_GAME_VER ||
        (major >= LATEST_MAJOR_GAME_VER && minor >= LATEST_MINOR_GAME_VER)
    ) {
        return "TU" + std::to_string(LATEST_TU + 1) + " or newer";
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
    // LOG_INFO(std::format("{} RegOpenKey {}", __FUNCTION__, subkey));

    LSTATUS open_status = RegOpenKey(HKEY_LOCAL_MACHINE, subkey.c_str(), &hKey);
    if (open_status != ERROR_SUCCESS) {
        LOG_ERROR(std::format("RegOpenKey failed. Status: {}", open_status));
    }
    
    const char* val_name = "Install Dir";

    char value_buf[1024];
    DWORD value_length = sizeof(value_buf);
    LSTATUS query_status = RegQueryValueEx(hKey, val_name, NULL, &dwType, reinterpret_cast<LPBYTE>(value_buf), &value_length);
    RegCloseKey(hKey);

    if (query_status != ERROR_SUCCESS) {
        LOG_ERROR(std::format("RegQueryValueEx failed. Status: {}", query_status));
        return fs::path("");
    }


    return fs::path(value_buf);
}

void Core::RunGame() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));

    std::string proc_name = g_Config.launch_values.game_proc_name;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring params = converter.from_bytes(g_Config.launch_values.params);

    if (g_Config.launch_values.is_trial) {
        proc_name = g_Config.launch_values.game_proc_name_trial;
        params += L" -trial";
    }

    std::filesystem::path game_full_path = GetGameInstallDir() / proc_name;

    if (fs::exists(game_full_path)) {
        auto sFullPath = ToUTF8String(game_full_path);
        LOG_INFO(std::format("game_full_path: {}", sFullPath));

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
            LOG_INFO(std::format("ShellExecuteExW Done {}. Error: {} {}", result, err, std::system_category().message(err).c_str()));
        }
        else {
            LOG_INFO("ShellExecuteExW Done");
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
            return false;
        }
    }
    catch (fs::filesystem_error const& e) {
        LOG_ERROR(std::format("Create Directory failed {} ({})", e.what(), ToUTF8String(d)));
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
    defaultLogger.SetFile(logFile);
}

fs::path Core::GetLEDataPathRegVal() {
    fs::path result;

    HKEY hKey;
    std::string key = std::string("SOFTWARE\\Live Editor\\FC ") + std::to_string(FIFA_EDITION) + "\\Data Dir";

    LOG_INFO(std::format("[{}] {}", __FUNCTION__, key));
    LSTATUS lOpenStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &hKey);
    if (lOpenStatus != ERROR_SUCCESS) {
        LOG_WARN(std::format("[{}] RegOpenKeyEx {} failed {}", __FUNCTION__, key, lOpenStatus));
        RegCloseKey(hKey);
        return result;
    }

    DWORD dwType = REG_SZ;
    wchar_t value_buf[1024];
    DWORD value_length = sizeof(value_buf);
    LSTATUS query_status = RegQueryValueExW(hKey, L"Data Dir", NULL, &dwType, reinterpret_cast<LPBYTE>(value_buf), &value_length);
    RegCloseKey(hKey);
    if (query_status != ERROR_SUCCESS) {
        LOG_WARN(std::format("[{}] RegQueryValueExW failed {}", __FUNCTION__, query_status));
        return fs::path("");
    }

    return fs::path(value_buf);
}

bool Core::SetLEPathRegVal(std::wstring data) {
    HKEY hKey;
    std::string key = std::string("SOFTWARE\\Live Editor\\FC ") + std::to_string(FIFA_EDITION) + "\\Dir";
    if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        LOG_WARN(std::format("[{}] RegCreateKeyExA failed", __FUNCTION__));
        return false;
    }
    else {
        DWORD cbData = static_cast<DWORD>((data.size() + static_cast<size_t>(1)) * sizeof(wchar_t));
        LSTATUS lSetStatus = RegSetValueExW(hKey, L"Dir", 0, REG_SZ, (LPBYTE)data.c_str(), cbData);
        RegCloseKey(hKey);

        if (lSetStatus != ERROR_SUCCESS)
        {
            LOG_WARN(std::format("[{}] RegSetValueExW failed {}", __FUNCTION__, lSetStatus));
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
        LOG_WARN(std::format("[{}] RegCreateKeyExA failed", __FUNCTION__));
        return false;
    }
    else {
        DWORD cbData = static_cast<DWORD>((data.size() + static_cast<size_t>(1)) * sizeof(wchar_t));
        LSTATUS lSetStatus = RegSetValueExW(hKey, L"Data Dir", 0, REG_SZ, (LPBYTE)data.c_str(), cbData);
        RegCloseKey(hKey);

        if (lSetStatus != ERROR_SUCCESS)
        {
            LOG_WARN(std::format("[{}] RegSetValueExW failed {}", __FUNCTION__, lSetStatus));
            return false;
        }
    }

    return true;
}

fs::path Core::GetLEDataPath() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));

    std::filesystem::path result = GetLEDataPathRegVal();

    if (!result.empty()) {
        LOG_INFO(std::format("[{}] Found Reg Val: {}", __FUNCTION__, ToUTF8String(result).c_str()));
        return result;
    }
    else {
        std::filesystem::path result(std::string(std::getenv("SystemDrive")) + "\\");
        result /= "FC " + std::to_string(FIFA_EDITION) + " Live Editor";

        if (!SetLEDataPathRegVal(result.wstring())) {
            LOG_WARN("SetLEDataPathRegVal failed");
        }

        LOG_INFO(std::format("[{}] Default: {}", __FUNCTION__, ToUTF8String(result)));
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
    LOG_INFO(std::format("[{}]", __FUNCTION__));
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
                LOG_ERROR("Can't find eaac backup");
            }
        }
    }
    catch (fs::filesystem_error const& e) {
        LOG_ERROR(std::format("Copy Fake EAAC error {}", e.what()));
    }

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
}

void Core::BackupOrgGameFiles() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));

    try {
        std::filesystem::path eaac_path = GetEAACLauncherPath();
        if (!eaac_path.empty()) {
            if (fs::file_size(eaac_path) > 0x1000000) {
                fs::path bak = GetBAKPathFor(eaac_path);
                if (fs::exists(bak))    fs::remove(bak);

                fs::copy(eaac_path, bak);
            }
        }
    }
    catch (fs::filesystem_error const& e) {
        LOG_ERROR(std::format("Backup EAAC error {}", e.what()));
    }

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
}
void Core::RestoreOrgGameFiles() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));

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
        LOG_ERROR(std::format("RestoreOrgGameFiles error {}", e.what()));
    }

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
}


void Core::SetupLocalize() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));
    localize.SetLangPath(ctx.GetFolder());
    localize.LoadLangTrans("EN");

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
}

void Core::ReadGameBuildInfo() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));
    std::filesystem::path build_info_dll_path = GetGameInstallDir() / "Engine.BuildInfo.dll";

    if (fs::exists(build_info_dll_path)) {
        LOG_INFO(std::format("Found {}", ToUTF8String(build_info_dll_path)));

        game_build_info = BuildInfo::GetInstance(build_info_dll_path);

        if (game_build_info) {
            LOG_INFO("----------------------[Frostbite BuildInfo]----------------------");
            LOG_INFO(std::format("BranchName:                         {}", game_build_info->getBranchName()));
            LOG_INFO(std::format("LicenseeId:                         {}", game_build_info->getLicenseeId()));
            LOG_INFO(std::format("StudioName:                         {}", game_build_info->getStudioName()));
            LOG_INFO(std::format("Changelist:                         {}", game_build_info->getChangelist()));
            LOG_INFO(std::format("SourceChangeList:                   {}", game_build_info->getSourceChangelist()));
            LOG_INFO(std::format("FrostbiteChangelist:                {}", game_build_info->getFrostbiteChangelist()));
            LOG_INFO(std::format("FrostbiteRelease:                   {}", game_build_info->getFrostbiteRelease()));
            LOG_INFO(std::format("IsAutoBuild:                        {}", game_build_info->getIsAutoBuild() ? "true" : "false"));
            LOG_INFO(std::format("Username:                           {}", game_build_info->getUsername()));
            LOG_INFO(std::format("BuildTime:                          {}", game_build_info->getBuildTime()));
            LOG_INFO(std::format("BuildDate:                          {}", game_build_info->getBuildDate()));
            LOG_INFO(std::format("BuildIsoDate:                       {}", game_build_info->getBuildIsoDate()));
            LOG_INFO("-----------------------------------------------------------------");
        }
    }

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
}

bool Core::InitDirectories() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));

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
            LOG_INFO(std::format("[{}] Copy legacy_structure.txt to {}", __FUNCTION__, ToUTF8String(dest_legacystructure).c_str()));
            std::filesystem::copy_file(cur_data_path / "legacy_structure.txt", dest_legacystructure);
        }

        if (!std::filesystem::exists(dest_dbmeta)) {
            LOG_INFO(std::format("[{}] Copy db_meta.xml to {}", __FUNCTION__, ToUTF8String(dest_dbmeta).c_str()));
            std::filesystem::copy_file(cur_data_path / "db_meta.xml", dest_dbmeta);
        }

        if (!std::filesystem::exists(dest_idmap)) {
            LOG_INFO(std::format("[{}] Copy id_map.json to {}", __FUNCTION__, ToUTF8String(dest_idmap).c_str()));
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

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
    return true;
}

void Core::CreateLegacyFilesStructure(std::filesystem::path folders_list, std::filesystem::path mods_dir) {
    LOG_INFO(std::format("[{}]", __FUNCTION__));
    
    std::filesystem::path legacy_root = mods_dir / "root";
    SafeCreateDirectories(legacy_root);
    legacy_root /= "Legacy";
    SafeCreateDirectories(legacy_root);

    std::ifstream folders_to_create(folders_list);
    std::string legacy_folder;
    std::filesystem::path full_path = "";
    while (std::getline(folders_to_create, legacy_folder)) {
        SafeCreateDirectories(legacy_root / legacy_folder);
    }

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
}

Core g_Core;