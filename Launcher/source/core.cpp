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

    LE::Config* le_config = LE::Config::GetInstance();

    LE::FilesManager* files_manager = LE::FilesManager::GetInstance();
    files_manager->SetupLogger();
    files_manager->CreateDirectories();
    files_manager->SetupConfig();

    LE::VersionManager* version_manager = LE::VersionManager::GetInstance();
    version_manager->Init();

    std::filesystem::path game_install_dir = files_manager->GetGameDirectory();

    LOG_INFO(std::format("{} {}", TOOL_NAME, version_manager->GetToolVersion()));
    LOG_INFO(std::format("Game Install Dir: {}", ToUTF8String(game_install_dir).c_str()));
    LOG_INFO(std::format("Live Editor Dir: {}", ToUTF8String(le_dir).c_str()));

    std::filesystem::path proc_full_path = files_manager->GetGameProcessFullPath();
    if (!std::filesystem::exists(proc_full_path)) {
        LOG_FATAL(std::format("Can't find file\n{}", ToUTF8String(proc_full_path).c_str()));
    }

    files_manager->InstallFakeAnticheat();
    files_manager->DetectFIFAModManager();
    version_manager->SetIsUsingCrackedGame(files_manager->DetectCracked());

    files_manager->SetInstallDir(le_dir);

    localize.Load();
    // g_options_ids.LoadJson();

    std::thread t1(&LE::VersionManager::CheckUpdates, version_manager);
    t1.detach();

    LOG_INFO(std::format("[{}] Done", __FUNCTION__));

    return true;
}

void Core::onExit() {
    LE::FilesManager::GetInstance()->UnInstallFakeAnticheat();
    ReleaseMutex(hMutex);
}

void Core::RunGame(bool no_mods) {
    LOG_INFO(std::format("[{}]", __FUNCTION__));

    LE::Config* le_config = LE::Config::GetInstance();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring params = converter.from_bytes(le_config->GetLauncherValues()->params);

    if (no_mods) {
        params = L" -no_mods";
    }

    if (!params.empty()) {
        LOG_INFO(std::format("Launch Options: {}", ToUTF8String(params).c_str()));
    }

    LE::FilesManager* files_manager = LE::FilesManager::GetInstance();
    fs::path game_full_path = files_manager->GetGameProcessFullPath();
    if (fs::exists(game_full_path)) {
        LOG_INFO(std::format("Game Location: {}", ToUTF8String(game_full_path).c_str()));

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
Core g_Core;