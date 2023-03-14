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

    fs::path GetGameInstallDir();
    void RunGame();
    bool isASCII(const std::string& s);

private:
    void SetupLogger();

    fs::path GetEAACLauncherPath();
    fs::path GetFakeEAACPath();
    fs::path GetBAKPathFor(fs::path p);

    void CopyFakeEAAC();
    void BackupOrgGameFiles();
    void RestoreOrgGameFiles();

};

extern Core g_Core;