#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <logger/logger.h>
#include <misc.h>
#include <../external/nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace core {
    class DirectoriesValues {
    public:
        // Default Values
        fs::path game_loc = "";
        fs::path mods_root = "";
        fs::path filters_storage = "";
        fs::path import_miniface = "";
        fs::path legacyfolder_export = "";
        fs::path legacyfile_export = "";
        fs::path legacyfile_import = "";

        void to_json(json& j);
        void from_json(const json& j);
    };

    class OverlayValues {
    public:
        // Default Values
        bool stop_draw_at_startup = false;
        bool hide_all_windows_at_startup = false;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class UIValues {
    public:
        // Default Values
        int scale = 0;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class LauncherValues {
    public:
        // Default Values
        bool show_disclaimer_msg = true;
        bool close_after_injection = true;
        bool auto_inject = true;
        bool is_trial = false;
        int injection_delay = 100;

        std::string game_proc_name = "FC25.exe";
        std::string game_proc_name_trial = "FC25_Trial.exe";
        std::string params = "";

        std::vector<std::string> dlls = {
            "FCLiveEditor.DLL"
        };

        void to_json(json& j);
        void from_json(const json& j);
    };

    class MatchFixingValues {
    public:
        int goals_scored = 3;
        int goals_conceded = 0;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class HotkeysValues {
    public:
        struct Hotkey {
            bool enabled = true;
            std::vector<int> keys_combination;

            Hotkey(std::vector<int> combination_arr) {
                keys_combination = combination_arr;
            }

            void to_json(json& j);
            void from_json(const json& j);
        };

        Hotkey show_menu_keys = Hotkey({ 122,0,0 });    // F11
        Hotkey hide_ui_keys =   Hotkey({ 120,0,0 });    // F9

        void to_json(json& j);
        void from_json(const json& j);
    };

    class LoggerValues {
    public:
        bool log_game_logs = false;
        bool log_try_find_chunk_file = false;
        bool log_iniread = false;
        int log_level = 1;
        bool log_load_chunk_file = false;
        bool luaL_tolstring = false;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class OtherValues {
    public:
        bool first_run = true;
        // bool disable_chants = true;
        bool show_player_potential = true;
        bool load_images = true;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class DEBUGValues {
    public:
        bool enable_crash_dump_gen = false;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class Config
    {
    public:
        std::string fname = "config.json";
        std::string imgui_ini = "launcher_imgui.ini";

        DirectoriesValues directories_values;
        OverlayValues overlay_values;
        UIValues ui_values;
        LauncherValues launch_values;
        MatchFixingValues matchfixing_values;
        HotkeysValues hotkeys_values;
        LoggerValues logger_values;
        OtherValues other_values;
        DEBUGValues debug_values;

        Config();
        ~Config();

        void Setup(std::filesystem::path folder);

        void Load();
        void Save();
    private:
        std::filesystem::path fpath;
        json o = json::object();

        void Create();

        void to_json(json& j);
        void from_json(const json& j);
    };
}

extern core::Config g_Config;
