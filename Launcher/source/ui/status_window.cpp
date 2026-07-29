#include "status_window.h"

namespace UIWindows {
    UIStatus::UIStatus() {}
    UIStatus::~UIStatus() {}

    void UIStatus::Draw(bool* p_open) {
        if (ImGui::Begin(GetWindowName(), p_open)) {
            LE::AuthManager* auth_manager = LE::AuthManager::GetInstance();
            if (auth_manager->GetLoginStatus() == LE::LOGIN_STATUS::NO_ACCESS) {
                DrawNoAccess();
            }
            else {
                DrawAccess();
            }

        }

        ImGui::End();

        FirstDraw();
    }


    void UIStatus::DrawNoAccess() {
        ImGui::Text("You don't have access");
        ImGui::TextCenter("Support this project on ", "Patreon", "https://www.patreon.com/checkout/xAranaktu?rid=4008263");
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 51, 51, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(192, 246, 241, 255));
        if (ImGui::Button("Logout", ImVec2(-FLT_MIN, 0.0f))) {
            LE::AuthManager::GetInstance()->DoLogout();
        }
        ImGui::PopStyleColor(2);
    }
    
    void UIStatus::DrawAccess() {
        LE::AuthManager* auth_manager = LE::AuthManager::GetInstance();

        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(GetInjectionStatusDesc().c_str());
        }
        ImGui::SameLine();
        ImGui::Text("STATUS: %s", GetInjectionStatus().c_str());

        if (g_Injector.GetStatus() == Injector::STATUS::STATUS_WAITING_FOR_GAME) {
            LE::Config* le_config = LE::Config::GetInstance();
            LE::LauncherValues* launch_values = le_config->GetLauncherValues();

            ImGui::Separator();

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

            LE::LOGIN_STATUS auth_status = auth_manager->GetLoginStatus();
            if (auth_status == LE::LOGIN_STATUS::HAS_ACCESS) {
                if (run_game_disabled) ImGui::BeginDisabled();

                if (
                    ImGui::Button("Run Game", ImVec2(-FLT_MIN, 0.0f)) &&
                    !run_game_pressed
                    ) {
                    run_game_pressed = true;
                    g_Core.RunGame(launch_values->no_mods);
                }

                ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 51, 51, 255));
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(192, 246, 241, 255));
                if (ImGui::Button("Logout", ImVec2(-FLT_MIN, 0.0f))) {
                    auth_manager->DoLogout();
                }
                ImGui::PopStyleColor(2);

                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Enable to see if your game run without mods");
                }
                ImGui::SameLine();

                if (ImGui::Checkbox("No Mods", &launch_values->no_mods)) {
                    le_config->Save();
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
                    ImGui::Text("Set Valid Game Location in Settings");
                }
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(138, 119, 23, 255));
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(192, 246, 241, 255));
                if (ImGui::Button("Login", ImVec2(-FLT_MIN, 0.0f))) {
                    UIWindows::UIAuthPopup::GetInstance()->Open();
                }
                ImGui::PopStyleColor(2);
            }

            ImGui::PopStyleVar();
        }
    }

    void UIStatus::Dock(ImGuiID dock_id) {
        ImGui::DockBuilderDockWindow(GetWindowName(), dock_id);
    }
    const char* UIStatus::GetWindowName() {
        return window_name.c_str(); 
    }

    void UIStatus::FirstDraw() {
        if (!first_draw)    return;

        std::filesystem::path game_full_path = LE::FilesManager::GetInstance()->GetGameProcessFullPath();

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
