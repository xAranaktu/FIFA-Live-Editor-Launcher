#include <gui.h>

GUI::GUI() {

}

GUI::~GUI() {

}

void GUI::Init() {
    if (initialized) return;
    sprintf(about_txt, "%s - %s", TOOL_NAME, TOOL_VERSION);

    about_window_size = ImVec2(
        1280.0f * 0.8f,
        800.0f - 30.0f
    );
    about_window_pos = ImVec2(
        static_cast<float>(GetSystemMetrics(SM_CXSCREEN) / 2),
        static_cast<float>(GetSystemMetrics(SM_CYSCREEN) / 2)
    );

    settings_window.Init();
    locale_window.Init();
    initialized = true;
}

void GUI::MainDockspace() {
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);


    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = GetMainDockspaceID();
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        if (create_dockspace_layout) {
            create_dockspace_layout = false;
            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            // split the dockspace into 2 nodes --
            // DockBuilderSplitNode takes in the following args in the following order
            //   window ID to split, direction, fraction (between 0 and 1),
            // the final two setting let's us choose which id we want (which ever one we DON'T set as NULL,
            // will be returned by the function)
            // out_id_at_dir is the id of the node in the direction we specified earlier,
            // out_id_at_opposite_dir is in the opposite direction
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, nullptr, &dockspace_id);
            auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.30f, nullptr, &dockspace_id);

            // we now dock our windows into the docking node we made above
            ImGui::DockBuilderDockWindow("Restart Required", dockspace_id);
            locale_window.Dock(dockspace_id);
            settings_window.Dock(dock_id_left);
            status_window.Dock(dock_id_left);
            ImGui::DockBuilderDockWindow("Info", dock_id_right);

            ImGui::DockBuilderFinish(dockspace_id);
        }
    }
    ImGui::End();

    DrawMainMenuBar();
}

void GUI::SetupImGUI() {
    LOG_FUNC_START();
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    {
        // Place the imgui.ini in the same directory where DLL is.
        auto imgui_ini_fp = LE::FilesManager::GetInstance()->GetImGuiIni();
        char* new_filename = new char[strlen(imgui_ini_fp) + 1];    // If we ever add ejecting dll... FIX MEMORY_LEAK
        strncpy(new_filename, imgui_ini_fp, strlen(imgui_ini_fp) + 1);
        io.IniFilename = new_filename;
    }

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    io.Fonts->AddFontDefault();

    ImFontConfig font_cfg = ImFontConfig();
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    font_cfg.SizePixels = 16.0f;
    font_cfg.MergeMode = true;
    font_cfg.PixelSnapH = true;

    io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_900_compressed_data, fa_solid_900_compressed_size, font_cfg.SizePixels, &font_cfg, icons_ranges);
    io.Fonts->Build();

    LOG_FUNC_END();
}

void GUI::DrawMainMenuBar() {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem(status_window.GetWindowName(), NULL, &status_window.show);
            ImGui::MenuItem(settings_window.GetWindowName(), NULL, &settings_window.show);

            if (locale_window.IsKeyLoaded()) {
                ImGui::MenuItem(locale_window.GetWindowName(), NULL, &locale_window.show);
            }

            ImGui::MenuItem("Info", NULL, &show_info_window);
            ImGui::MenuItem("Disclaimer", NULL, &show_disclaimer);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("Logs")) {
                ShellExecuteW(NULL, L"open", defaultLogger.GetFolder().wstring().c_str(), NULL, NULL, SW_SHOWDEFAULT);
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Open folder with Logs in Windows File Explorer");
            }

            ImGui::MenuItemURL("Wiki", "https://github.com/xAranaktu/FC-25-Live-Editor/wiki");
            ImGui::MenuItemURL("Getting Started", "https://github.com/xAranaktu/FC-25-Live-Editor/wiki/Getting-Started");
            ImGui::MenuItemURL("Discord", "https://discord.gg/yxAbpmNaFX");
            ImGui::MenuItem("About", NULL, &show_about);
            ImGui::EndMenu();
        }

        //if (ImGui::BeginMenu("Edit"))
        //{
        //    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
        //    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
        //    ImGui::Separator();
        //    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        //    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        //    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
        //    ImGui::EndMenu();
        //}
        ImGui::EndMainMenuBar();
    }
}

void GUI::DrawInfoWindow(bool* p_open) {
    ImGui::Begin("Info", p_open);
    ImGui::Text("LE Version             %s", g_Core.GetToolVer());

    ImGui::PushStyleColor(ImGuiCol_Text, compatibility_color);
    ImGui::Text("Game TU                %s", GetGameTU());
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(game_tu_desc.c_str());
    }

    ImGui::End();
}

void GUI::DrawAbout(bool* p_open) {
    ImGuiIO& io = ImGui::GetIO();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowPos(about_window_pos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(about_window_size, ImGuiCond_Appearing);
    ImGui::Begin("About", p_open, window_flags);
    ImVec2 avail_space = ImGui::GetContentRegionAvail();

    ImGui::BeginGroup();

    ImGui::TextCenter(std::string(about_txt));
    ImGui::TextCenter("Created by ", "Aranaktu", "https://github.com/xAranaktu");

    ImGui::EndGroup();

    ImGui::BeginChild("#3rdPartyLibs", ImVec2(avail_space.x, avail_space.y * 0.40f));
    ImGui::TextCenter("Used Libraries:");
    ImGui::InputTextMultiline("##3rdPartyLibs Content", &third_party_libs, ImVec2(avail_space.x, -10), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly);
    ImGui::EndChild();

    ImGui::BeginChild("#Credits", ImVec2(avail_space.x, avail_space.y * 0.40f));

    ImGui::InputTextMultiline("##Credits Content", &credits, ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly);
    ImGui::EndChild();

    ImGui::TextCenter("Support this project on ", "Patreon", "https://www.patreon.com/xAranaktu");

    ImGui::End();
}

void GUI::Draw() {
    MainDockspace();

    if (restart_required) {
        MessageBox(NULL, "Live Editor Launcher requires restart to apply the changes.", "Restart Required", MB_ICONINFORMATION);
        restart_required = false;
    }

    if (show_demo_window)       ImGui::ShowDemoWindow(&show_demo_window);
    if (locale_window.show)     locale_window.Draw(&locale_window.show);
    if (status_window.show)     status_window.Draw(&status_window.show);
    if (settings_window.show)   settings_window.Draw(&settings_window.show);
    if (show_info_window)       DrawInfoWindow(&show_info_window);
    if (show_about)             DrawAbout(&show_about);

    if (first_draw) {
        ImGui::SetWindowFocus(status_window.GetWindowName());
        first_draw = false;
    }

    FileDialogs();
    LE::EditHotkeyWindow::GetInstance()->Draw();
}

bool GUI::GameVerIsCompatibleWithLE(std::string TU) {
    for (const auto compatible_tu : COMPATIBLE_TITLE_UPDATES) {
        if (TU == compatible_tu)    return true;
    }

    return false;
}

std::string GUI::GetGameTU() {
    if (!game_tu.empty())
        return game_tu;

    game_tu = g_Core.GetTU();
    if (game_tu == "Invalid") {
        game_tu_desc = "Live Editor wasn't able to determine game version\nThis tool may not work properly with your game version";

        // Orange
        compatibility_color = IM_COL32(255, 165, 0, 255);
    }
    else if (game_tu.size() >= 10) {
        // game_tu_desc = "Your game version is newer than latest known TU by the Live Editor\nThis tool may not work properly with your game version";

        // Lime
        compatibility_color = IM_COL32(0, 255, 0, 255);
    }
    else if (GameVerIsCompatibleWithLE(game_tu)) {
        game_tu_desc = "Your game version is compatible with the Live Editor";

        // Lime
        compatibility_color = IM_COL32(0, 255, 0, 255);
    }
    else {
        game_tu_desc = "Your game version isn't compatible with the Live Editor";

        // Red
        compatibility_color = IM_COL32(255, 0, 0, 255);
    }

    return game_tu;
}

void GUI::ChangeModsRootDialog() {
    // display
    if (ImGuiFileDialog::Instance()->Display("ModsRootFD", ImGuiWindowFlags_NoCollapse, fd_min))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::filesystem::path fpath(ImGuiFileDialog::Instance()->GetCurrentPath());

            if (fs::exists(fpath) && fs::is_directory(fpath)) {
                LE::FilesManager::GetInstance()->SetLEModsDir(fpath);
            }

            restart_required = true;
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}

void GUI::ChangeGameLoc() {
    if (ImGuiFileDialog::Instance()->Display("GameLocFD", ImGuiWindowFlags_NoCollapse, fd_min))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::filesystem::path fpath(ImGuiFileDialog::Instance()->GetCurrentPath());

            if (fs::exists(fpath) && fs::is_directory(fpath)) {
                LE::FilesManager::GetInstance()->SetCustomGameDir(fpath);
            }

            restart_required = true;
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}

void GUI::ChangeLEDataRootDialog() {
    // display
    if (ImGuiFileDialog::Instance()->Display("LEDataRootFD", ImGuiWindowFlags_NoCollapse, fd_min))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::filesystem::path fpath(ImGuiFileDialog::Instance()->GetCurrentPath());

            if (fs::exists(fpath) && fs::is_directory(fpath)) {
                LE::FilesManager::GetInstance()->SetLEDataDir(fpath);
            }

            restart_required = true;
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}

void GUI::FileDialogs() {
    std::string key = ImGuiFileDialog::Instance()->GetOpenedKey();
    if (key.empty()) return;
    if (fd_map.count(key) != 1) return;

    GUI::FILE_DIALOGS opened_dialog = fd_map.at(key);
    switch (opened_dialog)
    {
    case GUI::FILE_DIALOGS::FILE_DIALOG_GAME_LOC:
        ChangeGameLoc();
        break;
    case GUI::FILE_DIALOGS::FILE_DIALOG_LE_DATA_ROOT:
        ChangeLEDataRootDialog();
        break;
    case GUI::FILE_DIALOGS::FILE_DIALOG_MODS_ROOT:
        ChangeModsRootDialog();
        break;
    default:
        break;
    }
}

void GUI::CloseCurrentFileDialog() {
    std::string opened_key = ImGuiFileDialog::Instance()->GetOpenedKey();

    if (opened_key.empty()) return;
    if (!ImGuiFileDialog::Instance()->IsOpened())   return;

    // close
    ImGuiFileDialog::Instance()->Close();
}

GUI g_GUI;