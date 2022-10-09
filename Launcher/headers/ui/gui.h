#pragma once
#include <config.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "injector_window.h"

// Main User Interface
class GUI
{
public:
    bool initialized = false;
    bool create_dockspace_layout = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool show_demo_window = false;
    bool show_info_window = true;
    bool show_disclaimer = true;

    UIWindows::UIInjector injector_window;

    GUI();
    ~GUI();

    void Init();
    void MainDockspace();
    void SetupImGUI();
    void DrawMainMenuBar();
    void DrawInfoWindow(bool* p_open);
    void DrawDisclaimer(bool* p_open);
    void Draw();

private:
    ImGuiID GetMainDockspaceID() { return ImGui::GetID("LauncherMainDockSpace"); };
};

extern GUI g_GUI;