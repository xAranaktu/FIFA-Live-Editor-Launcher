#pragma once

#include <string> 
#include <sstream>
#include <filesystem>
#include <locale> 
#include <codecvt> 

#include <misc.h>
#include <logger/logger.h>
#include <consts.h>
#include <config.h>
#include <context.h>
#include <Psapi.h>
#include <TlHelp32.h>

#include "tinyxml2.h"

#include "misc.h"
#include "options_id.h"
#include "localize.h"

#include <files_manager/files_manager.h>

namespace fs = std::filesystem;

// Hearth & Soul of our app 
class Core
{
public:
    core::Context ctx;
    HANDLE hMutex = NULL;

    Core();
    ~Core();

    bool Init();
    void onExit();

    const char* GetToolVer();
    std::string GetGameVer();
    std::string GetTU();

    // Create Live Editor Folders and Files in C:/ (or other drive, depending on result of std::getenv("SystemDrive"))
    bool InitDirectories();

    void RunGame();

private:
    void SetupLocalize();
};

extern Core g_Core;