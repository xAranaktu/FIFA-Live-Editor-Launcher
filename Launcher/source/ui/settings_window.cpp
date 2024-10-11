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

        if (ImGuiLoc::CollapsingHeader("advanced_launch_options")) {
            if (ImGui::InputText("##advlaunchoptions", &launch_values->params)) {
                save_required |= true;
            }
        }


        if (ImGuiLoc::CollapsingHeader("dll_injector")) {
            if (ImGuiLoc::InputInt("inject_delay", &launch_values->injection_delay)) {
                save_required |= true;
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("The delay between game process found and the injection of Live Editor core dll\nToo short may cause problems/crashes\nToo long may result in some mods not being loaded properly");
            }

            if (ImGuiLoc::Checkbox("auto_close_launcher", &launch_values->close_after_injection)) {
                save_required |= true;
            }

            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Live Editor will shutdown itself after successfull DLL injection. (recommended)");
            }

            if (ImGuiLoc::Checkbox("is_trial", &launch_values->is_trial)) {
                save_required |= true;
            }

            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Trial game is the game version available for 10h (with EA Play, but not EA Play Pro). Tick this checkbox if you are using Auto Run Game and you have trial game version");
            }
        }

        if (ImGuiLoc::CollapsingHeader("logger")) {
            if (ImGuiLoc::Combo("log_level", &current_log_level, avail_log_levels)) {
                defaultLogger.SetMinLevel(current_log_level);
                save_required |= true;
            }

            LE::LoggerValues* logger_values = le_config->GetLoggerValues();

            save_required |= ImGui::Checkbox(localize.Translate("log_try_find_legacy_file").c_str(), &logger_values->log_try_find_legacy_file);
            save_required |= ImGui::Checkbox(localize.Translate("log_load_legacy_file").c_str(), &logger_values->log_load_legacy_file);
        }

        if (ImGuiLoc::CollapsingHeader("directories")) {
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

        if (ImGuiLoc::CollapsingHeader("ui")) {
            LE::UIValues* ui_values = le_config->GetUIValues();

            if (ImGuiLoc::CollapsingHeader("attr_colors")) {
                LE::UIValues* ui_values = le_config->GetUIValues();

                save_required |= AttrColor("elite", &ui_values->attr_elite);
                save_required |= AttrColor("excellent", &ui_values->attr_excellent);
                save_required |= AttrColor("good", &ui_values->attr_good);
                save_required |= AttrColor("average", &ui_values->attr_average);
                save_required |= AttrColor("poor", &ui_values->attr_poor);
            }

            if (ImGuiLoc::Combo("scale", &ui_values->scale, avail_scale_factors)) {
                g_GUI.scale_changed = true;
                save_required |= true;
            }
        }

        if (ImGuiLoc::CollapsingHeader("overlay")) {
            LE::OverlayValues* overlay_values = le_config->GetOverlayValues();

            save_required |= ImGuiLoc::Checkbox("show_overlay_at_startup", &overlay_values->show_overlay_at_startup);
            ImGui::BasicTooltip("show_overlay_at_startup_tooltip");
        }

        ImGui::PushID("hotkeys_collapsing");
        if (ImGuiLoc::CollapsingHeader("hotkeys")) {
            LE::HotkeyManager* hotkey_manager = LE::HotkeyManager::GetInstance();
            HotkeyEntry(hotkey_manager->GetHotkeyAction(LE::HotkeyActionID::ACTION_SHOW_UI));
        }
        ImGui::PopID();

        if (ImGuiLoc::CollapsingHeader("launcher")) {
            save_required |= ImGuiLoc::Checkbox("show_warning_at_startup", &launch_values->show_disclaimer_msg);
            ImGui::BasicTooltip("show_warning_at_startup_tooltip");
        }

        if (ImGuiLoc::CollapsingHeader("transfers")) {
            LE::TransfersValues* transfer_values = le_config->GetTransfersValues();

            save_required |= ImGuiLoc::Checkbox("auto_delete_presigned_contracts", &transfer_values->auto_delete_presigned_contracts);
            ImGui::BasicTooltip("auto_delete_presigned_contracts_tooltip");

            save_required |= ImGuiLoc::Checkbox("auto_terminate_loans", &transfer_values->auto_terminate_loans);
            ImGui::BasicTooltip("auto_terminate_loans_tooltip");
        }

        if (ImGuiLoc::CollapsingHeader("other")) {
            LE::OtherValues* other_values = le_config->GetOtherValues();

            save_required |= ImGuiLoc::Checkbox("show_player_potential", &other_values->show_player_potential);
            ImGui::BasicTooltip("show_player_potential_tooltip");

            save_required |= ImGuiLoc::Checkbox("auto_reload_images", &other_values->auto_reload_images);
            ImGui::BasicTooltip("auto_reload_images_tooltip");
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

    bool UISettings::AttrColor(const char* name, LESetting::Attribute* attr) {
        bool save_required = false;

        std::string bgid = std::format("attr_colors_{}_bg", name);
        std::string txtid = std::format("attr_colors_{}_txt", name);
        std::string thresholdid = std::format("attr_colors_{}_threshold", name);

        ImGui::PushID(bgid.c_str());
        save_required |= ImGuiLoc::ColorEdit3(bgid.c_str(), reinterpret_cast<float*>(&attr->bg_color));
        ImGui::PopID();

        ImGui::PushID(txtid.c_str());
        save_required |= ImGuiLoc::ColorEdit3(txtid.c_str(), reinterpret_cast<float*>(&attr->txt_color));
        ImGui::PopID();

        ImGui::PushID(thresholdid.c_str());
        save_required |= ImGuiLoc::InputInt(thresholdid.c_str(), &attr->threshold);
        ImGui::PopID();

        return save_required;
    }
}

