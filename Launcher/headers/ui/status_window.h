#pragma once
#include <thread>
#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

#include "injector.h"

namespace UIWindows {
    class UIStatus
    {
    public:
        bool show = true;
        std::string window_name = "Status";

        UIStatus();
        ~UIStatus();

        void Draw(bool* p_open);
        void Dock(ImGuiID dock_id);
        const char* GetWindowName();

        void FirstDraw();
        void InjectDll();
        std::string GetInjectionStatus();
        std::string GetInjectionStatusDesc();

    private:
        bool first_draw = true;
        bool run_game_pressed = false;

        bool run_game_disabled = true;
        std::string run_game_disabled_reason;
    };
}
