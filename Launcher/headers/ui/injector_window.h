#pragma once
#include <thread>
#include <string>
#include <vector>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

#include "injector.h"

namespace UIWindows {
    class UIInjector
    {
    public:
        enum Platform {
            Origin = 0,
            EAApp,
            Origin_Steam,
            EAApp_Steam,
        };

        bool show = true;
        std::string window_name = "Injector";

        UIInjector();
        ~UIInjector();
        void Draw(bool* p_open);
        void Dock(ImGuiID dock_id);
        const char* GetWindowName();

        void InjectDll();
        std::string GetInjectionStatus();
        std::string GetInjectionStatusDesc();

    private:
        int picked_platform = Platform::EAApp;
        std::vector<std::string> Platform_strings = {
            "Origin", "EA App", "Steam & Origin", "Steam & EA App"
        };
    };
}
