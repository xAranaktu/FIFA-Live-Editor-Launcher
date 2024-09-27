#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <logger/logger.h>
#include <misc.h>
#include <consts.h>

#include <../external/nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace LE {
    class DirectoriesValues {
    public:
        // Default Values
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

        // Default values in Config::Init
        std::string game_proc_name;
        std::string game_proc_name_trial;
        std::string params;

        std::vector<std::string> dlls = {
            "FCLiveEditor.DLL"
        };

        void to_json(json& j);
        void from_json(const json& j);
    };

    class HotkeysValues {
    public:
        struct Hotkey {
            bool enabled = true;
            std::vector<int> keys_combination;

            Hotkey(std::vector<int> combination_arr) {
                SetCombination(combination_arr);
            }

            void SetCombination(std::vector<int> combination_arr) {
                keys_combination = combination_arr;
            }

            void SetCombination(std::vector<uint8_t> combination_arr) {
                keys_combination.clear();
                for (auto key : combination_arr) {
                    keys_combination.push_back(key);
                }
            }

            void to_json(json& j);
            void from_json(const json& j);
        };

        Hotkey show_ui_keys =   Hotkey({ 120 });    // F9

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
        Config(Config& other) = delete;
        void operator=(const Config&) = delete;
        static Config* GetInstance();

        void to_json(json& j);
        void from_json(const json& j);

        void Init(fs::path path);

        void Load();
        void Save();

        LauncherValues* GetLauncherValues() { return &launch_values; }
        HotkeysValues* GetHotkeyValues() { return &hotkeys_values; }
        UIValues* GetUIValues() { return &ui_values; }
        OverlayValues* GetOverlayValues() { return &overlay_values; }
        OtherValues* GetOtherValues() { return &other_values; }
        LoggerValues* GetLoggerValues() { return &logger_values; }

        bool IsTrial() { return launch_values.is_trial; }
        bool ShowDisclaimer() { return launch_values.show_disclaimer_msg; }
        bool AutoInject() { return launch_values.auto_inject; }
        bool CloseAfterInjection() { return launch_values.close_after_injection; }
        int GetInjectionDelay() { return launch_values.injection_delay; }

        std::vector<std::string> GetDlls() { return launch_values.dlls; }

        std::string GetParams() { return launch_values.params; }
        std::string GetProcName() { return launch_values.game_proc_name; }
        std::string GetProcNameTrial() { return launch_values.game_proc_name_trial; }

    private:
        static Config* pinstance_;
        static std::mutex mutex_;

        DirectoriesValues directories_values;
        OverlayValues overlay_values;
        UIValues ui_values;
        LauncherValues launch_values;
        HotkeysValues hotkeys_values;
        LoggerValues logger_values;
        OtherValues other_values;
        DEBUGValues debug_values;

        const std::string imgui_filename = "le_launcher_imgui.ini";
       
        fs::path cfg_path;

    protected:
        Config();
        ~Config();
    };
}
