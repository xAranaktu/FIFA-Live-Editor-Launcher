#include "settings_window.h"
#include "gui.h"

namespace UIWindows {
    UISettings::UISettings() {}
    UISettings::~UISettings() {}

    void UISettings::Init() {
        for (auto s : logger.levelStrings) {
            avail_log_levels.push_back(localize.Translate(s.c_str()));
        };

        has_keys_options = !g_options_ids.GetOptions("keys", true).empty();
    }

    void UISettings::Draw(bool* p_open) {
        bool save_required = false;

        ImGui::Begin(GetWindowName(), p_open);

        if (ImGui::CollapsingHeader(localize.Translate("Logger").c_str())) {
            if (ImGui::Combo(localize.Translate("log_level").c_str(), &current_log_level, avail_log_levels)) {
                logger.SetMinLevel((LogLevel)current_log_level);
                save_required |= true;
            }

            save_required |= ImGui::Checkbox(localize.Translate("log_try_find_chunk_file").c_str(), &g_Config.logger_values.log_try_find_chunk_file);
            save_required |= ImGui::Checkbox(localize.Translate("log_load_chunk_file").c_str(), &g_Config.logger_values.log_load_chunk_file);
            save_required |= ImGui::Checkbox(localize.Translate("log_iniread").c_str(), &g_Config.logger_values.log_iniread);
            save_required |= ImGui::Checkbox(localize.Translate("log_game_logs").c_str(), &g_Config.logger_values.log_game_logs);

            if (ImGui::Checkbox(localize.Translate("log_luaL_tolstring").c_str(), &g_Config.logger_values.luaL_tolstring)) {
                save_required |= true;
            }
        }

        if (ImGui::CollapsingHeader(localize.Translate("Directories").c_str())) {
            ImGui::Text("LE Data Root: ");
            ImGui::SameLine();
            if (ImGui::Button("...")) {
                g_GUI.CloseCurrentFileDialog();
                ImGuiFileDialog::Instance()->OpenDialog("LEDataRootFD", "Choose a Directory", nullptr, ".");
            }
            ImGui::SameLine();
            ImGui::Text(ToUTF8String(g_Core.GetLEDataPath()).c_str());

            ImGui::Text("Mods Root: ");
            ImGui::SameLine();
            if (ImGui::Button("...")) {
                g_GUI.CloseCurrentFileDialog();
                ImGuiFileDialog::Instance()->OpenDialog("ModsRootFD", "Choose a Directory", nullptr, ".");
            }
            ImGui::SameLine();
            ImGui::Text(ToUTF8String(g_Config.directories_values.mods_root).c_str());
        }

        if (ImGui::CollapsingHeader(localize.Translate("UI").c_str())) {
            if (ImGui::Combo(localize.Translate("Scale").c_str(), &g_Config.ui_values.scale, avail_scale_factors)) {
                g_GUI.scale_changed = true;
                save_required |= true;
            }
        }

        if (ImGui::CollapsingHeader(localize.Translate("Overlay").c_str())) {
            save_required |= ImGui::Checkbox(localize.Translate("stop_draw_at_startup").c_str(), &g_Config.overlay_values.stop_draw_at_startup);
            save_required |= ImGui::Checkbox(localize.Translate("hide_all_windows_at_startup").c_str(), &g_Config.overlay_values.hide_all_windows_at_startup);
        }

        ImGui::PushID("hotkeys_collapsing");
        if (ImGui::CollapsingHeader(localize.Translate("Hotkeys").c_str()) && has_keys_options) {
            HotkeyMultiCombo("Show Menu: ", "multi_show_menu_key", g_Config.hotkeys_values.show_menu_keys);
            HotkeyMultiCombo("Hide UI: ", "multi_hide_ui_key", g_Config.hotkeys_values.hide_ui_keys);
        }
        ImGui::PopID();

        if (ImGui::CollapsingHeader(localize.Translate("Match-Fixing").c_str())) {
            save_required |= ImGui::InputInt("Goals Scored", &g_Config.matchfixing_values.goals_scored);
            save_required |= ImGui::InputInt("Goals Conceded", &g_Config.matchfixing_values.goals_conceded);
        }

        if (ImGui::CollapsingHeader(localize.Translate("Launcher").c_str())) {
            save_required |= ImGui::Checkbox("Show Warning at startup", &g_Config.launch_values.show_disclaimer_msg);

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Shows the Disclaimer Warning at Live Editor Launcher start.");
        }

        if (ImGui::CollapsingHeader(localize.Translate("Other").c_str())) {
            if (ImGui::Checkbox(localize.Translate("disable_chants_short").c_str(), &g_Config.other_values.disable_chants)) {
                save_required |= true;
            }

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(localize.Translate("disable_chants").c_str());

            save_required |= ImGui::Checkbox(localize.Translate("show_player_potential").c_str(), &g_Config.other_values.show_player_potential);
            save_required |= ImGui::Checkbox(localize.Translate("load_images_short").c_str(), &g_Config.other_values.load_images);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(localize.Translate("load_images").c_str());

        }

        if (save_required) {
            g_Config.Save();
        }

        ImGui::End();
    }

    void UISettings::Dock(ImGuiID dock_id) {
        ImGui::DockBuilderDockWindow(GetWindowName(), dock_id);
    }
    const char* UISettings::GetWindowName() {
        return window_name.c_str();
    }

    void UISettings::HotkeyMultiCombo(const char* label, std::string id, core::HotkeysValues::Hotkey& for_hotkey) {
        float font_sz = ImGui::GetFontSize();
        auto& style = ImGui::GetStyle();

        ImGui::BeginGroup();
        ImGui::BeginChild(id.c_str(), ImVec2(0, font_sz + (style.ItemInnerSpacing.y * 6)), true, ImGuiWindowFlags_NoScrollbar);
        auto avail_space = ImGui::GetContentRegionAvail();

        if (ImGui::Checkbox(label, &for_hotkey.enabled)) {
            g_Config.Save();
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
                    logger.Write(LOG_ERROR, "[%s] [LCOMBO] Unknown key: %d", __FUNCTION__, for_hotkey.keys_combination[i]);
                }

                if (ImGui::LCombo("", &for_hotkey.keys_combination[i], available_keys_to_assign, translate, combo_id)) {
                    g_Config.Save();
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

