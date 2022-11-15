#pragma once

#include <string> 
#include <sstream>
#include <filesystem>

#include <logger.h>
#include <consts.h>
#include <config.h>
#include <context.h>
#include <Psapi.h>
#include <TlHelp32.h>

namespace fs = std::filesystem;

// Hearth & Soul of our app 
class Core
{
public:
    core::Context ctx;

    Core();
    ~Core();

    bool Init();
    void onExit();
    const char* GetToolVer();

    std::string GetGameInstallDir();
    void RunGame();
    void ToggleAnticheat(bool disable);
    bool isASCII(const std::string& s);

    void ApplyPlatform(bool isOrigin);

private:
    void SetupLogger();

    std::string GetInstallerDataPath();
    std::string GetEAACLauncherPath();

    void CopyFakeEAAC();
    void BackupOrgGameFiles();
    void RestoreOrgGameFiles();

};

extern Core g_Core;