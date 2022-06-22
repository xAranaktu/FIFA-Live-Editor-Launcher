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
        enum DirectX {
            DX12 = 0,
            DX11 = 1
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

    private:
        bool auto_inject = false;
        int picked_direcx = DirectX::DX12;
        std::vector<std::string> DX_strings = {
            "DX12", "DX11"
        };
    };
}
