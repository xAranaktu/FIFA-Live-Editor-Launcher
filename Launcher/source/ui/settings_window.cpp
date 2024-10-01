#include "settings_window.h"
#include "gui.h"

namespace UIWindows {
    UISettings::UISettings() {}
    UISettings::~UISettings() {}

    void UISettings::Init() {
        current_log_level = defaultLogger.GetLogLevel();
        avail_log_levels.clear();
        for (const auto l : logger_level_strings) {
            avail_log_levels.push_back(l);
        }

        has_keys_options = !g_options_ids.GetOptions("keys", true).empty();
    }

    void UISettings::Draw(bool* p_open) {
        bool save_required = false;
        LE::Config* le_config = LE::Config::GetInstance();
        LE::FilesManager* files_manager = LE::FilesManager::GetInstance();

        LE::LauncherValues* launch_values = le_config->GetLauncherValues();

        ImGui::Begin(GetWindowName(), p_open);

        if (ImGui::CollapsingHeader(localize.Translate("Advanced Launch Options").c_str())) {
            if (ImGui::InputText("##advlaunchoptions", &launch_values->params)) {
                save_required |= true;
            }
        }

        if (ImGui::CollapsingHeader(localize.Translate("DLL Injector").c_str())) {
            if (ImGui::InputInt("Delay (ms)", &launch_values->injection_delay)) {
                save_required |= true;
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("The delay between game process found and the injection of Live Editor core dll\nToo short may cause problems/crashes\nToo long may result in some mods not being loaded properly");
            }

            if (ImGui::Checkbox("Auto Close Launcher", &launch_values->close_after_injection)) {
                save_required |= true;
            }

            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Live Editor will shutdown itself after successfull DLL injection. (recommended)");
            }

            if (ImGui::Checkbox("Is Trial Game", &launch_values->is_trial)) {
                save_required |= true;
            }

            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Trial game is the game version available for 10h (with EA Play, but not EA Play Pro). Tick this checkbox if you are using Auto Run Game and you have trial game version");
            }
        }

        if (ImGui::CollapsingHeader(localize.Translate("Logger").c_str())) {
            if (ImGui::Combo("Log Level", &current_log_level, avail_log_levels)) {
                defaultLogger.SetMinLevel(current_log_level);
                save_required |= true;
            }

            LE::LoggerValues* logger_values = le_config->GetLoggerValues();

            save_required |= ImGui::Checkbox(localize.Translate("log_try_find_legacy_file").c_str(), &logger_values->log_try_find_legacy_file);
            save_required |= ImGui::Checkbox(localize.Translate("log_load_legacy_file").c_str(), &logger_values->log_load_legacy_file);
        }

        if (ImGui::CollapsingHeader(localize.Translate("Directories").c_str())) {
            ImGui::Text("Game Location:     ");
            ImGui::SameLine();
            ImGui::PushID("##GameLocFDBTN");
            if (ImGui::Button("...")) {
                g_GUI.CloseCurrentFileDialog();

                IGFD::FileDialogConfig cfg;
                cfg.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("GameLocFD", "Choose a Directory", nullptr, cfg);
            }
            ImGui::PopID();
            ImGui::SameLine();

            ImGui::Text(files_manager->GetGameDirectoryU8().c_str());

            ImGui::Text("LE Data Root:      ");
            ImGui::SameLine();
            ImGui::PushID("##LEDataRootFDBTN");
            if (ImGui::Button("...")) {
                g_GUI.CloseCurrentFileDialog();

                IGFD::FileDialogConfig cfg;
                cfg.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("LEDataRootFD", "Choose a Directory", nullptr, cfg);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::Text(files_manager->GetLEDataDirectoryU8().c_str());

            ImGui::Text("Mods Root:         ");
            ImGui::SameLine();
            ImGui::PushID("##ModsRootFDBTN");
            if (ImGui::Button("...")) {
                g_GUI.CloseCurrentFileDialog();

                IGFD::FileDialogConfig cfg;
                cfg.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("ModsRootFD", "Choose a Directory", nullptr, cfg);
            }
            ImGui::PopID();
            ImGui::SameLine();

            ImGui::Text(files_manager->GetLEModsDirectoryU8().c_str());
        }

        if (ImGui::CollapsingHeader(localize.Translate("UI").c_str())) {
            LE::UIValues* ui_values = le_config->GetUIValues();

            if (ImGui::Combo(localize.Translate("Scale").c_str(), &ui_values->scale, avail_scale_factors)) {
                g_GUI.scale_changed = true;
                save_required |= true;
            }
        }

        ImGui::PushID("hotkeys_collapsing");
        if (ImGui::CollapsingHeader(localize.Translate("Hotkeys").c_str())) {
            LE::HotkeyManager* hotkey_manager = LE::HotkeyManager::GetInstance();
            HotkeyEntry(hotkey_manager->GetHotkeyAction(LE::HotkeyActionID::ACTION_SHOW_UI));
        }
        ImGui::PopID();

        if (ImGui::CollapsingHeader(localize.Translate("Launcher").c_str())) {
            save_required |= ImGui::Checkbox("Show Warning at startup", &launch_values->show_disclaimer_msg);
            
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Shows the Disclaimer Warning at Live Editor Launcher start.");
        }

        if (ImGui::CollapsingHeader(localize.Translate("Other").c_str())) {
            LE::OtherValues* other_values = le_config->GetOtherValues();

            save_required |= ImGui::Checkbox(localize.Translate("show_player_potential").c_str(), &other_values->show_player_potential);
            // save_required |= ImGui::Checkbox(localize.Translate("auto_reload_images").c_str(), &other_values->load_images);
            // if (ImGui::IsItemHovered())
            //     ImGui::SetTooltip(localize.Translate("auto_reload_images_tooltip").c_str());

        }

        if (save_required) {
            le_config->Save();
        }

        ImGui::End();
    }

    void UISettings::Dock(ImGuiID dock_id) {
        ImGui::DockBuilderDockWindow(GetWindowName(), dock_id);
    }
    const char* UISettings::GetWindowName() {
        return window_name.c_str();
    }

    void UISettings::HotkeyEntry(LE::HotkeyAction* action) {
        std::string hotkey_name = action->GetName();

        ImGui::PushID(hotkey_name.c_str());
        if (ImGui::Button(ICON_FA_PEN_TO_SQUARE)) {
            LE::EditHotkeyWindow::GetInstance()->Open(action);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
            ImGui::SetTooltip("Click to edit hotkey");
            ImGui::PopStyleVar();
        }

        ImGui::SameLine();

        ImGui::Text(std::format("{} [{}]", hotkey_name.c_str(), action->GetCombination().c_str()).c_str());
        if (ImGui::IsItemHovered()) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
            ImGui::SetTooltip(action->GetDescription().c_str());
            ImGui::PopStyleVar();
        }

        ImGui::PopID();
    }
}

