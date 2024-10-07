#pragma once
#include <filesystem>
#include <vector>
#include <string>

#include <misc.h>
#include <config/settings.h>
#include <../external/nlohmann/json.hpp>

#define COLRGB(value) ( (float)value / 255.0f)

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace LE {
    class DirectoriesValues {
    public:
        fs::path mods_root = "";
        fs::path game_loc = "";
        fs::path filters_storage = "";
        fs::path import_miniface = "";
        fs::path legacyfolder_export = "";
        fs::path legacyfile_export = "";
        fs::path legacyfile_import = "";

        void to_json(json& j);
        void from_json(const json& j);
    };

    class UIValues {
    public:
        // Default Values
        int scale = 0;

        LESetting::Attribute attr_elite = LESetting::Attribute(90, LESetting::Color3(COLRGB(0), COLRGB(60), COLRGB(0)), LESetting::Color3(COLRGB(255), COLRGB(255), COLRGB(255)));
        LESetting::Attribute attr_excellent = LESetting::Attribute(80, LESetting::Color3(COLRGB(0), COLRGB(94), COLRGB(0)), LESetting::Color3(COLRGB(255), COLRGB(255), COLRGB(255)));
        LESetting::Attribute attr_good = LESetting::Attribute(70, LESetting::Color3(COLRGB(154), COLRGB(205), COLRGB(50)), LESetting::Color3(COLRGB(0), COLRGB(0), COLRGB(0)));
        LESetting::Attribute attr_average = LESetting::Attribute(60, LESetting::Color3(COLRGB(255), COLRGB(165), COLRGB(0)), LESetting::Color3(COLRGB(255), COLRGB(255), COLRGB(255)));
        LESetting::Attribute attr_poor = LESetting::Attribute(50, LESetting::Color3(COLRGB(255), COLRGB(0), COLRGB(0)), LESetting::Color3(COLRGB(255), COLRGB(255), COLRGB(255)));


        void to_json(json& j);
        void from_json(const json& j);
    };

    class OverlayValues {
    public:
        bool show_overlay_at_startup = true;

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
        LESetting::Hotkey show_ui_keys = LESetting::Hotkey({ 120 });    // F9

        void to_json(json& j);
        void from_json(const json& j);
    };

    class MessageBoxesValues {
    public:
        std::vector<std::string> dont_show_messageboxes;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class LoggerValues {
    public:
        int log_level = 1;
        bool log_try_find_legacy_file = false;
        bool log_load_legacy_file = false;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class TransfersValues {
    public:
        bool auto_delete_presigned_contracts = false;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class OtherValues {
    public:
        bool first_run = true;
        bool show_player_potential = true;
        bool auto_reload_images = false;

        void to_json(json& j);
        void from_json(const json& j);
    };

    class DEBUGValues {
    public:
        bool enable_crash_dump_gen = false;

        void to_json(json& j);
        void from_json(const json& j);
    };
}