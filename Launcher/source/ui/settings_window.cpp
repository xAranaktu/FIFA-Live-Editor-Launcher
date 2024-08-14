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

            save_required |= ImGui::Checkbox(localize.Translate("log_try_find_chunk_file").c_str(), &logger_values->log_try_find_chunk_file);
            save_required |= ImGui::Checkbox(localize.Translate("log_load_chunk_file").c_str(), &logger_values->log_load_chunk_file);
            save_required |= ImGui::Checkbox(localize.Translate("log_iniread").c_str(), &logger_values->log_iniread);
            save_required |= ImGui::Checkbox(localize.Translate("log_game_logs").c_str(), &logger_values->log_game_logs);

            if (ImGui::Checkbox(localize.Translate("log_luaL_tolstring").c_str(), &logger_values->luaL_tolstring)) {
                save_required |= true;
            }
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

        if (ImGui::CollapsingHeader(localize.Translate("Overlay").c_str())) {
            LE::OverlayValues* overlay_values = le_config->GetOverlayValues();
            save_required |= ImGui::Checkbox(localize.Translate("stop_draw_at_startup").c_str(), &overlay_values->stop_draw_at_startup);
            save_required |= ImGui::Checkbox(localize.Translate("hide_all_windows_at_startup").c_str(), &overlay_values->hide_all_windows_at_startup);
        }

        if (ImGui::CollapsingHeader(localize.Translate("Launcher").c_str())) {
            save_required |= ImGui::Checkbox("Show Warning at startup", &launch_values->show_disclaimer_msg);
            
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Shows the Disclaimer Warning at Live Editor Launcher start.");
        }

        if (ImGui::CollapsingHeader(localize.Translate("Other").c_str())) {
            LE::OtherValues* other_values = le_config->GetOtherValues();

            save_required |= ImGui::Checkbox(localize.Translate("show_player_potential").c_str(), &other_values->show_player_potential);
            save_required |= ImGui::Checkbox(localize.Translate("load_images_short").c_str(), &other_values->load_images);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(localize.Translate("load_images").c_str());

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

    void UISettings::HotkeyMultiCombo(const char* label, std::string id, LE::HotkeysValues::Hotkey& for_hotkey) {
        float font_sz = ImGui::GetFontSize();
        auto& style = ImGui::GetStyle();

        ImGui::BeginGroup();
        ImGui::BeginChild(id.c_str(), ImVec2(0, font_sz + (style.ItemInnerSpacing.y * 6)), true, ImGuiWindowFlags_NoScrollbar);
        auto avail_space = ImGui::GetContentRegionAvail();

        if (ImGui::Checkbox(label, &for_hotkey.enabled)) {
            // LE::Config::GetInstance()->Save();
        }
        ImGui::SameLine();
        if (for_hotkey.enabled) {
            float hotkey_item_width = 65.0f;
            //float item_cposx = avail_space.x - hotkey_item_width;
            bool translate = false;
            auto available_keys_to_assign = g_options_ids.GetOptions("keys", false);
            for (int i = 0; i < 3; i++) {
                std::string combo_id = id + std::to_string(i);
                //ImGui::SetCursorPos(ImVec2(item_cposx, style.ItemInnerSpacing.y * 2));
                ImGui::BeginGroup();
                ImGui::BeginChild(combo_id.c_str(), ImVec2(hotkey_item_width, font_sz + (style.ItemInnerSpacing.y * 6)), false, ImGuiWindowFlags_NoScrollbar);

                if (available_keys_to_assign.count(for_hotkey.keys_combination[i]) != 1) {
                    // LOG_ERROR(std::format("[{}] [LCOMBO] Unknown key: {}", __FUNCTION__, for_hotkey.keys_combination[i]));
                }

                if (ImGui::LCombo("", &for_hotkey.keys_combination[i], available_keys_to_assign, translate, combo_id)) {
                    // LE::Config::GetInstance()->Save();
                }
                ImGui::EndChild();
                ImGui::EndGroup();
                if (i < 2) {
                    ImGui::SameLine();
                    ImGui::Text("+");
                    ImGui::SameLine();
                    //if (ImGui::IsItemHovered()) {
                    //    ImGui::SetTooltip("X: %0.2f, Y: %0.2f", plus_ico_sz.x, plus_ico_sz.y);
                    //}

                    //item_cposx -= font_sz;
                }
                //item_cposx -= hotkey_item_width;
            }
        }
        else {
            std::string disabled_txt = "<DISABLED>";
            float disabled_width = ImGui::CalcTextSize(disabled_txt.c_str()).x;
            ImGui::SameLine();
            ImGui::SetCursorPosX(avail_space.x - disabled_width - font_sz * 2 - style.ItemInnerSpacing.x);
            ImGui::Text(disabled_txt.c_str());
        }

        ImGui::EndChild();
        ImGui::EndGroup();
    }
}

