#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <logger.h>
#include <../external/nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace core {
    class Config
    {
    public:
        std::string imgui_ini = "launcher_imgui.ini";
        bool auto_inject = true;
        int injection_delay = 100;
        std::vector<std::string> proc_names = {
            "FIFA22.exe",
            "FIFA22_Trial.exe"
        };

        std::vector<std::string> dlls = {
            "FIFALiveEditor.DLL"
        };

        Config();
        ~Config();

        void Setup(std::string folder);

        void Load();
        void Save();
    private:
        std::string fpath;
        json o = json::object();

        void LoadFromStrArray(std::vector<std::string>& to, std::string from);
    };
}

extern core::Config g_Config;
