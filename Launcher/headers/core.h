#pragma once

#include <string> 
#include <sstream>
#include <filesystem>

#include <logger.h>
#include <config.h>
#include <context.h>

namespace fs = std::filesystem;

// Hearth & Soul of our app 
class Core
{
public:
    core::Context ctx;

    Core();
    ~Core();

    void Init();

private:
    void SetupLogger();

};

extern Core g_Core;