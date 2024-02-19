#pragma once
#include <config.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_helper.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "../external/ImGuiFileDialog/ImGuiFileDialog.h"
#include "status_window.h"
#include "settings_window.h"
#include "locale_window.h"

// Main User Interface
class GUI
{
public:
    // FileDialog
    enum FILE_DIALOGS {
        FILE_DIALOG_LE_DATA_ROOT = 0,
        FILE_DIALOG_MODS_ROOT,
        FILE_DIALOG_GAME_LOC,
        FILE_DIALOG_IMPORTMINIFACE,
        FILE_DIALOG_OPEN_LUA,
        FILE_DIALOG_SAVE_LUA,
        FILE_DIALOG_IMPORT_LEGACY_FILE,
        FILE_DIALOG_EXPORT_LEGACY_FILE,
        FILE_DIALOG_EXPORT_LEGACY_TO_FOLDER,
        FILE_DIALOG_IMPORT_FILTER_LIST,
        FILE_DIALOG_EXPORT_FILTER_LIST
    };
    ImVec2 fd_min = ImVec2(640.0f, 360.0f);

    bool initialized = false;
    bool first_draw = true;
    bool create_dockspace_layout = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool restart_required = false;
    bool scale_changed = false;

    bool show_demo_window = false;
    bool show_info_window = true;
    bool show_disclaimer = true;
    bool show_about = false;

    UIWindows::UIStatus status_window;
    UIWindows::UISettings settings_window;
    UIWindows::UILocaleIni locale_window;

    GUI();
    ~GUI();

    void Init();
    void MainDockspace();
    void SetupImGUI();
    void DrawMainMenuBar();
    void DrawInfoWindow(bool* p_open);
    void DrawAbout(bool* p_open);
    void Draw();

    void FileDialogs();

    void CloseCurrentFileDialog();

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
        "ImGuiNotify            https://github.com/TyomaVader/ImGuiNotify \n"
        "Font Awesome 6         https://github.com/juliettef/IconFontCppHeaders \n"
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
        "tokke001                   for help with players traits\n"
        "FIFA Community who shares details about the game and making the game better with their mods over many years\n"
        "\nAnd everyone who decided to support me on Patreon. :)\n"
        "\n";

    std::map<std::string, GUI::FILE_DIALOGS> fd_map = {
        { "LEDataRootFD",           GUI::FILE_DIALOGS::FILE_DIALOG_LE_DATA_ROOT },
        { "ModsRootFD",             GUI::FILE_DIALOGS::FILE_DIALOG_MODS_ROOT },
        { "GameLocFD",              GUI::FILE_DIALOGS::FILE_DIALOG_GAME_LOC },
        { "ImportMinifaceFD",       GUI::FILE_DIALOGS::FILE_DIALOG_IMPORTMINIFACE },
        { "LoadLuaFD",              GUI::FILE_DIALOGS::FILE_DIALOG_OPEN_LUA },
        { "SaveLuaFD",              GUI::FILE_DIALOGS::FILE_DIALOG_SAVE_LUA },
        { "ImportLegacyFile",       GUI::FILE_DIALOGS::FILE_DIALOG_IMPORT_LEGACY_FILE },
        { "ExportLegacyFile",       GUI::FILE_DIALOGS::FILE_DIALOG_EXPORT_LEGACY_FILE },
        { "ExportLegacyToFolder",   GUI::FILE_DIALOGS::FILE_DIALOG_EXPORT_LEGACY_TO_FOLDER },
        { "ExportFilterList",       GUI::FILE_DIALOGS::FILE_DIALOG_EXPORT_FILTER_LIST },
        { "ImportFilterList",       GUI::FILE_DIALOGS::FILE_DIALOG_IMPORT_FILTER_LIST }
    };

    std::string game_tu = "";
    std::string game_tu_desc = "";
    ImU32 compatibility_color = IM_COL32(0, 0, 0, 255);

    bool GameVerIsCompatibleWithLE(std::string TU);
    std::string GetGameTU();
    void ChangeModsRootDialog();
    void ChangeLEDataRootDialog();
    void ChangeGameLoc();
    ImGuiID GetMainDockspaceID() { return ImGui::GetID("LauncherMainDockSpace"); };
};

extern GUI g_GUI;