#pragma once

#include <string> 
#include <sstream>
#include <filesystem>

#include <misc.h>
#include <logger.h>
#include <consts.h>
#include <config.h>
#include <context.h>
#include <Psapi.h>
#include <TlHelp32.h>

#include <SDK/FB/build_info.h>

#include "options_id.h"
#include "localize.h"

namespace fs = std::filesystem;

// Hearth & Soul of our app 
class Core
{
public:
    core::Context ctx;
    BuildInfo* game_build_info = nullptr;
    HANDLE hMutex = NULL;

    Core();
    ~Core();

    bool Init();
    void onExit();

    void DetectFIFAModManager();

    const char* GetToolVer();

    fs::path GetGameInstallDir();
    fs::path GetLEDataPath();
    bool SetLEPathRegVal(std::wstring data);
    bool SetLEDataPathRegVal(std::wstring data);
    fs::path GetLEDataPathRegVal();

    // Create Live Editor Folders and Files in C:/ (or other drive, depending on result of std::getenv("SystemDrive"))
    bool InitDirectories();


    void RunGame();

private:
    bool SafeCreateDirectories(const std::filesystem::path d);
    void SetupLogger();

    fs::path GetEAACLauncherPath();
    fs::path GetFakeEAACPath();
    fs::path GetBAKPathFor(fs::path p);

    void CopyFakeEAAC();
    void BackupOrgGameFiles();
    void RestoreOrgGameFiles();

    void SetupLocalize();
    void ReadGameBuildInfo();

    // Create FIFA Legacy files structure
    void CreateLegacyFilesStructure(std::filesystem::path folders_list, std::filesystem::path mods_dir);
};

extern Core g_Core;