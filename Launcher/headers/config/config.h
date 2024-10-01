#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <logger/logger.h>
#include <consts.h>

#include <config/values.h>
#include <config/ui_styles.h>

#include <../external/nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace LE {
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

        UIValues* GetUIValues() { return &ui_values; }
        LauncherValues* GetLauncherValues() { return &launch_values; }
        HotkeysValues* GetHotkeyValues() { return &hotkeys_values; }
        LoggerValues* GetLoggerValues() { return &logger_values; }
        MessageBoxesValues* GetMessageBoxesValues() { return &messageboxes_values; }
        TransfersValues* GetTransfersValues() { return &transfers_values; }
        OtherValues* GetOtherValues() { return &other_values; }
        DEBUGValues* GetDEBUGValues() { return &debug_values; }
        DirectoriesValues* GetDirectoriesValues() { return &directories_values; }
        UIStyle* GetUIStyle() { return &ui_style; }

    private:
        static Config* pinstance_;
        static std::mutex mutex_;

        UIValues ui_values;
        LauncherValues launch_values;
        HotkeysValues hotkeys_values;
        LoggerValues logger_values;
        MessageBoxesValues messageboxes_values;
        TransfersValues transfers_values;
        OtherValues other_values;
        DEBUGValues debug_values;
        DirectoriesValues directories_values;
        UIStyle ui_style;

        fs::path cfg_path;

    protected:
        Config();
        ~Config();
    };
}
