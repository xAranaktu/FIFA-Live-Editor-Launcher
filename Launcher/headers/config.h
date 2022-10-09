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
        std::string fname = "le_config.json";
        std::string imgui_ini = "launcher_imgui.ini";
        bool show_disclaimer_msg = true;
        bool auto_inject = true;
        bool auto_start = true;
        bool is_trial = false;
        int injection_delay = 300;
        std::vector<std::string> proc_names;
        std::vector<std::string> window_class_names;
        std::vector<std::string> dlls;

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
