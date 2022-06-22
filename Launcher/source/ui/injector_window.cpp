#include "injector_window.h"

namespace UIWindows {
    UIInjector::UIInjector() {}
    UIInjector::~UIInjector() {}

    void UIInjector::Draw(bool* p_open) {
        ImGui::Begin(GetWindowName(), p_open);

        ImGui::Text("STATUS: %s", GetInjectionStatus().c_str());

        if (ImGui::Checkbox("Auto Inject", &g_Config.auto_inject)) {
            g_Config.Save();
        }

        if (!auto_inject) {
            ImGui::SameLine();
            if (ImGui::Button("Inject")) {
                InjectDll();
            }
        }
        else {
            if (ImGui::InputInt("Injection Delay (ms)", &g_Config.injection_delay)) {
                g_Config.Save();
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
        std::thread t1(&Injector::Inject, &g_Injector);
        t1.detach();
    }

    std::string UIInjector::GetInjectionStatus() {
        return g_Injector.GetStatusName();
    }
}
