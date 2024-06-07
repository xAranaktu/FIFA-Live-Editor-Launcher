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

            if (run_game_disabled) ImGui::BeginDisabled();

            if (
                ImGui::Button("Run Game", ImVec2(-FLT_MIN, 0.0f)) &&
                !run_game_pressed
            ) {
                run_game_pressed = true;
                g_Core.RunGame();
            }

            if (run_game_disabled) {
                ImGui::EndDisabled();

                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip(run_game_disabled_reason.c_str());
                }

                ImGui::SameLine();
                ImGui::Text("Can't find game exe");
            }

            ImGui::PopStyleVar();
        }

        ImGui::End();

        FirstDraw();
    }

    void UIStatus::Dock(ImGuiID dock_id) {
        ImGui::DockBuilderDockWindow(GetWindowName(), dock_id);
    }
    const char* UIStatus::GetWindowName() {
        return window_name.c_str(); 
    }

    void UIStatus::FirstDraw() {
        if (!first_draw)    return;

        std::string proc_name = g_Config.launch_values.game_proc_name;
        std::filesystem::path game_full_path = g_Core.GetGameInstallDir() / proc_name;

        if (fs::exists(game_full_path)) {
            run_game_disabled = false;
        }
        else {
            run_game_disabled = true;
            run_game_disabled_reason = std::format("Can't find game in {}\n\nYou can still try to run the game manually from EAApp/Steam/Epic and it should work fine!", ToUTF8String(game_full_path));
        }

        first_draw = false;
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
