#pragma once

#include <string> 
#include <sstream>
#include <filesystem>
#include <locale> 
#include <codecvt> 

#include <misc.h>
#include <logger/logger.h>
#include <consts.h>
#include <config/config.h>
#include <context.h>
#include <Psapi.h>
#include <TlHelp32.h>

#include "misc.h"
#include "options_id.h"
#include "localize.h"

#include <files_manager/files_manager.h>
#include <version_manager/version_manager.h>

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

    void RunGame(bool no_mods);

private:

};

extern Core g_Core;