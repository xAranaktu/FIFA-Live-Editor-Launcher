#pragma once

#include <mutex>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include <format>

#include "../external/ImGuiFileDialog/ImGuiFileDialog.h"
#include <config/config.h>
#include "hotkey_manager.h"

namespace LE {
    class CreateHotkeyWindow {
    public:
        CreateHotkeyWindow(CreateHotkeyWindow& other) = delete;
        void operator=(const CreateHotkeyWindow&) = delete;
        static CreateHotkeyWindow* GetInstance();

        void Open();
        void Draw();
        void SetLUAScriptPath(std::filesystem::path fpath);

    private:
        static CreateHotkeyWindow* pinstance_;
        static std::mutex mutex_;

        bool show = false;

        LE::HotkeyAction current_action;
        std::string default_path;
        std::string lua_script;
        std::string error;

        std::string name;
        std::string description;
        std::string new_combination;

        int current_hotkey_action_type = 0;
        std::vector<int> keys;
        std::vector<std::string> avail_keys;
        std::vector<std::string> avail_action_types;
        std::map<int, std::string> avail_key_names;

        std::chrono::system_clock::time_point input_last_change;
        bool text_input_changed = false;

        std::string GetKeyName(unsigned char code);
        void UpdateCombination();

        void SaveHotkey();
        void CloseCurrentFileDialog();

    protected:
        CreateHotkeyWindow();
        ~CreateHotkeyWindow();
    };
}