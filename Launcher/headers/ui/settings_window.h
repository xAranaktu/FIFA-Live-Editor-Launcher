#pragma once
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif  // IMGUI_DEFINE_MATH_OPERATORS

#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "../external/IconsFontAwesome6.h"
#include "../external/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../external/nlohmann/fifo_map.hpp"

#include <hotkey_manager.h>
#include <edit_hotkey_window.h>
#include "localize.h"
#include <logger/logger.h>
#include <config/config.h>

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
        int current_log_level = 0;
        std::vector<std::string> avail_log_levels;
        std::vector<std::string> avail_scale_factors = {
            "auto", "x1.0", "x1.25", "x1.5", "x1.75", "x2.0", "x2.25"
        };

        void HotkeyEntry(LE::HotkeyAction* action);

        bool has_keys_options = false;

    };
}