#include "status_window.h"

namespace UIWindows {
    UIStatus::UIStatus() {}
    UIStatus::~UIStatus() {}

    void UIStatus::Draw(bool* p_open) {
        ImGui::Begin(GetWindowName(), p_open);

        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(GetInjectionStatusDesc().c_str());
        }
        ImGui::SameLine();
        ImGui::Text("STATUS: %s", GetInjectionStatus().c_str());

        if (g_Injector.GetStatus() == Injector::STATUS::STATUS_WAITING_FOR_GAME) {
            ImGui::Separator();
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
            if (
                ImGui::Button("Run Game", ImVec2(-FLT_MIN, 0.0f)) &&
                !run_game_pressed
            ) {
                run_game_pressed = true;
                g_Core.RunGame();
            }

            ImGui::PopStyleVar();
        }

        ImGui::End();
    }

    void UIStatus::Dock(ImGuiID dock_id) {
        ImGui::DockBuilderDockWindow(GetWindowName(), dock_id);
    }
    const char* UIStatus::GetWindowName() {
        return window_name.c_str(); 
    }

    void UIStatus::InjectDll() {
        g_Injector.SetDelay(g_Config.launch_values.injection_delay);
        std::thread t1(&Injector::Inject, &g_Injector);
        t1.detach();
    }

    std::string UIStatus::GetInjectionStatus() {
        return g_Injector.GetStatusName();
    }

    std::string UIStatus::GetInjectionStatusDesc() {
        return g_Injector.GetStatusDesc();
    }
}
