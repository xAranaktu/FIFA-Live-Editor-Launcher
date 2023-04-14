#pragma once
#include <config.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_helper.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "injector_window.h"
#include "locale_window.h"

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
    bool show_about = false;

    UIWindows::UIInjector injector_window;
    UIWindows::UILocaleIni locale_window;

    GUI();
    ~GUI();

    void Init();
    void MainDockspace();
    void SetupImGUI();
    void DrawMainMenuBar();
    void DrawInfoWindow(bool* p_open);
    void DrawDisclaimer(bool* p_open);
    void DrawAbout(bool* p_open);
    void Draw();

private:
    // ABOUT
    ImVec2 about_window_pos;
    ImVec2 about_window_size;
    char about_txt[256] = { '\0' };
    std::string third_party_libs =
        "Dear ImGui             https://github.com/ocornut/imgui \n"
        "PolyHook 2.0           https://github.com/stevemk14ebr/PolyHook_2_0 \n"
        "TinyXML2               https://github.com/leethomason/tinyxml2 \n"
        "Json                   https://github.com/nlohmann/json \n"
        "AES                    https://github.com/SergeyBel/AES \n"
        "ImGuiFileDialog        https://github.com/aiekick/ImGuiFileDialog \n"
        "Font Awesome 5         https://github.com/juliettef/IconFontCppHeaders \n"
        "DDSTextureLoader       https://github.com/Microsoft/DirectXTK/wiki/DDSTextureLoader \n"
        "Launcher               https://github.com/xAranaktu/FIFA-Live-Editor-Launcher \n"
        "Font - Fira Mono       https://fonts.google.com/specimen/Fira+Mono \n"
        "\n";

    std::string credits =
        "[CREDITS]\n\n"
        "IChooseYou (https://www.unknowncheats.me/forum/members/278477.html) for:\nFrostbite Engine Research, SDK Generator, File Dumper, Pattern Scanner, Code snippets \n"
        "\n\n[SPECIAL THANKS]\n\n"
        "GalaxyMan2015 & benji      for Frosty Tool Suite\n"
        "Rinaldo                    for Database Master 15 & Revolution DB Master\n"
        "FIFA Community who shares details about the game and making the game better with their mods over many years\n"
        "\nAnd everyone who decided to support me on Patreon. :)\n"
        "\n";

    ImGuiID GetMainDockspaceID() { return ImGui::GetID("LauncherMainDockSpace"); };
};

extern GUI g_GUI;