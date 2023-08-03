#pragma once
#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "../external/IconsFontAwesome5.h"
#include "../external/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../external/nlohmann/fifo_map.hpp"

#include "localize.h"
#include "logger.h"
#include "config.h"

namespace UIWindows {
    class UISettings {
    public:
        bool show = true;
        std::string window_name = "Settings";

        UISettings();
        ~UISettings();

        void Init();
        void Draw(bool* p_open);
        void Dock(ImGuiID dock_id);
        const char* GetWindowName();

    private:
        std::vector<std::string> avail_log_levels;
        std::vector<std::string> avail_scale_factors = {
            "auto", "x1.0", "x1.25", "x1.5", "x1.75", "x2.0", "x2.25"
        };

        std::string le_data_root;
        std::string game_loc;

        bool has_keys_options = false;
        void HotkeyMultiCombo(const char* label, std::string id, core::HotkeysValues::Hotkey& for_hotkey);

    };
}