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

        //if (ImGui::Checkbox("Auto Inject", &g_Config.auto_inject)) {
        //    if (g_Config.auto_inject) {
        //        InjectDll();
        //    }
        //    else {
        //        // Stop the auto injection loop
        //        g_Injector.SetInterupt(true);
        //    }

        //    g_Config.Save();
        //}

        //if (ImGui::IsItemHovered()) {
        //    ImGui::SetTooltip("If enable the launcher will automatically attempt to find the game process and inject the Live Editor core dll into it.");
        //}

        if (!g_Config.launch_values.auto_inject) {
            if (ImGui::Button("Inject")) {
                InjectDll();
            }
        }
        else {
            if (ImGui::InputInt("Delay (ms)", &g_Config.launch_values.injection_delay)) {
                g_Injector.SetDelay(g_Config.launch_values.injection_delay);
                g_Config.Save();
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("The delay between game process found and the injection of Live Editor core dll\nToo short may cause problems/crashes\nToo long may result in some mods not being loaded properly");
            }
        }

        //if (ImGui::Combo("Platform", &picked_platform, Platform_strings)) {
        //    g_Config.platform_origin = (picked_platform == Platform::Origin || picked_platform == Platform::Origin_Steam);
        //    g_Core.ApplyPlatform(g_Config.platform_origin);
        //    g_Config.Save();
        //}

        if (ImGui::Button("Run Game")) {
            if (g_Injector.GetStatus() != Injector::STATUS::STATUS_WAITING_FOR_GAME) {
                MessageBox(NULL, "You can't run the game multiple times.\nRestart the Launcher.", "Multirun Attempt", MB_ICONERROR);
            }
            else {
                g_Core.RunGame();
            }
        }

        if (ImGui::Checkbox("Auto Run Game", &g_Config.launch_values.auto_start)) {
            g_Config.Save();
        }
        
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Auto start the game with Live Editor (requires restart).\nFor EA Play you need to tick the 'Is Trial Game' to make it work.");
        }

        if (ImGui::Checkbox("Auto Close Launcher", &g_Config.launch_values.close_after_injection)) {
            g_Config.Save();
        }

        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Live Editor will shutdown itself after successfull DLL injection. (recommended)");
        }
        
        if (ImGui::Checkbox("Is Trial Game", &g_Config.launch_values.is_trial)) {
            g_Config.Save();
        }
        
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Trial game is the game version available for 10h (with EA Play, but not EA Play Pro). Tick this checkbox if you are using Auto Run Game and you have trial game version");
        }

        ImGui::End();
    }

    void UIInjector::Dock(ImGuiID dock_id) {
        ImGui::DockBuilderDockWindow(GetWindowName(), dock_id);
    }
    const char* UIInjector::GetWindowName() {
        return window_name.c_str(); 
    }

    void UIInjector::InjectDll() {
        g_Injector.SetDelay(g_Config.launch_values.injection_delay);
        std::thread t1(&Injector::Inject, &g_Injector);
        t1.detach();
    }

    std::string UIInjector::GetInjectionStatus() {
        return g_Injector.GetStatusName();
    }

    std::string UIInjector::GetInjectionStatusDesc() {
        return g_Injector.GetStatusDesc();
    }
}
