#pragma once

#include <mutex>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include <format>

#include <config/config.h>
#include "hotkey_manager.h"

namespace LE {
    class EditHotkeyWindow {
    public:
        EditHotkeyWindow(EditHotkeyWindow& other) = delete;
        void operator=(const EditHotkeyWindow&) = delete;
        static EditHotkeyWindow* GetInstance();

        void Open(LE::HotkeyAction* action);

        void Draw();

    private:
        static EditHotkeyWindow* pinstance_;
        static std::mutex mutex_;

        bool show = false;
        LE::HotkeyAction* current_action = nullptr;

        std::string new_combination;

        std::vector<int> keys;
        std::vector<std::string> avail_keys;
        std::map<int, std::string> avail_key_names;

        std::string GetKeyName(unsigned char code);
        void UpdateCombination();

        void SaveHotkey();

    protected:
        EditHotkeyWindow();
        ~EditHotkeyWindow();
    };
}