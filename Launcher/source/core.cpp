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

    LE::FilesManager* files_manager = LE::FilesManager::GetInstance();
    files_manager->SetupLogger();
    files_manager->CreateDirectories();

    std::filesystem::path game_install_dir = files_manager->GetGamePath();

    LOG_INFO(std::format("{} {}", TOOL_NAME, TOOL_VERSION));
    LOG_INFO(std::format("Game Install Dir: {}", ToUTF8String(game_install_dir).c_str()));
    LOG_INFO(std::format("Live Editor Dir: {}", ToUTF8String(le_dir).c_str()));

    std::string procname = std::format("FC{}.exe", EAFC_EDITION);
    std::filesystem::path proc_full_path = game_install_dir / procname;
    if (!std::filesystem::exists(proc_full_path)) {
        std::string msg = std::format("Can't find {} in :\n{}", procname, ToUTF8String(game_install_dir));
        LOG_FATAL(msg);
    }

    files_manager->InstallFakeAnticheat();

    SetupLocalize();

    g_options_ids.LoadJson();

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));

    return true;
}

void Core::onExit() {
    LE::FilesManager::GetInstance()->UnInstallFakeAnticheat();
    ReleaseMutex(hMutex);
}

const char* Core::GetToolVer() {
    return TOOL_VERSION;
}

std::string Core::GetGameVer() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));
    std::string result = "0.0.0.0";

    auto game_loc = LE::FilesManager::GetInstance()->GetGamePath();
    if (game_loc.empty() || !fs::exists(game_loc)) {
        return result;
    }

    fs::path fpath = game_loc / "__Installer" / "installerdata.xml";
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

    // auto ver_splitted = splitStr(game_ver, ".");
    // if (ver_splitted.size() != 4) {
    //     return "Invalid";
    // }
    // 
    // int minor = std::stoi(ver_splitted[ver_splitted.size() - 1]);
    // int major = std::stoi(ver_splitted[ver_splitted.size() - 2]);
    // 
    // if (
    //     major > LATEST_MAJOR_GAME_VER ||
    //     (major >= LATEST_MAJOR_GAME_VER && minor >= LATEST_MINOR_GAME_VER)
    // ) {
    //     return "TU" + std::to_string(LATEST_TU + 1) + " or newer";
    // }

    return game_ver;
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

    auto game_loc = LE::FilesManager::GetInstance()->GetGamePath();
    if (game_loc.empty() || !fs::exists(game_loc)) {
        return;
    }

    std::filesystem::path game_full_path = game_loc / proc_name;

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

void Core::SetupLocalize() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));
    localize.SetLangPath(ctx.GetFolder());
    localize.LoadLangTrans("EN");

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
}

bool Core::InitDirectories() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));

    // g_Config.Load();

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));
    return true;
}
Core g_Core;