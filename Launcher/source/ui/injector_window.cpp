#include "injector_window.h"

namespace UIWindows {
    UIInjector::UIInjector() {}
    UIInjector::~UIInjector() {}

    void UIInjector::Draw(bool* p_open) {
        ImGui::Begin(GetWindowName(), p_open);

        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(GetInjectionStatusDesc().c_str());
        }
        ImGui::Text("STATUS: %s", GetInjectionStatus().c_str());

        //if (ImGui::Button("Run Game")) {
        //    g_Core.RunGame();
        //}

        if (ImGui::Checkbox("Auto Inject", &g_Config.auto_inject)) {
            if (g_Config.auto_inject) {
                InjectDll();
            }
            else {
                // Stop the auto injection loop
                g_Injector.SetInterupt(true);
            }

            g_Config.Save();
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("If enable the launcher will automatically attempt to find the game process and inject the Live Editor core dll into it.");
        }

        if (!g_Config.auto_inject) {
            if (ImGui::Button("Inject")) {
                InjectDll();
            }
        }
        else {
            if (ImGui::InputInt("Delay (ms)", &g_Config.injection_delay)) {
                g_Config.Save();
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("The delay between game process found and the injection of Live Editor core dll\nToo short may cause problems/crashes\nToo long may result in some mods not being loaded properly");
            }
        }

        //if (ImGui::CollapsingHeader("fifasetup")) {
        //    if (ImGui::Combo("DirectX", &picked_direcx, DX_strings)) {
        //        // TODO: Save DirectX in fifasetup.ini
        //    }
        //}

        ImGui::End();
    }

    void UIInjector::Dock(ImGuiID dock_id) {
        ImGui::DockBuilderDockWindow(GetWindowName(), dock_id);
    }
    const char* UIInjector::GetWindowName() {
        return window_name.c_str(); 
    }

    void UIInjector::InjectDll() {
        std::thread t1(&Injector::Inject, &g_Injector, g_Config.injection_delay);
        t1.detach();
    }

    std::string UIInjector::GetInjectionStatus() {
        return g_Injector.GetStatusName();
    }

    std::string UIInjector::GetInjectionStatusDesc() {
        return g_Injector.GetStatusDesc();
    }
}
