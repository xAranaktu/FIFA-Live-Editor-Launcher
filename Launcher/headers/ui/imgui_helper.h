#pragma once
#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif  // IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_stdlib.h>
#include <config/config.h>
#include "localize.h"
#include "../external/nlohmann/fifo_map.hpp"

namespace ImGui {
    inline bool LCombo(std::string lbl, int* currIndex, nlohmann::fifo_map<int, std::string>& values, bool translate_items, std::string id = "", float lbl_width = 222.0f) {
        if (values.empty()) {
            return false;
        }
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 0.0f));

        auto idx = *currIndex;

        auto& style = ImGui::GetStyle();

        ImGui::BeginGroup();
        if (id.empty()) {
            ImGui::PushID(lbl.c_str());
        }
        else {
            ImGui::PushID(id.c_str());
        }
        ImVec2 sz = ImVec2(0.0f, ImGui::GetFrameHeight() + (style.WindowPadding.y * 2));
        ImGui::BeginChild("", sz, false, ImGuiWindowFlags_NoScrollbar);

        auto avail = ImGui::GetContentRegionAvail();
        auto cpos = ImGui::GetCursorPos();

        // ImGui::SetCursorPos(ImVec2(cpos.x + avail.x - style.ItemSpacing.x, cpos.y));
        ImGui::PushItemWidth(avail.x);

        auto trans = values.at(idx);
        if (translate_items) {
            trans = localize.Translate(trans.c_str());
        }

        const char* preview_value = trans.c_str();
        bool value_changed = false;
        if (ImGui::BeginCombo("", preview_value, 0)) {

            // Display items
            // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
            ImGuiListClipper clipper;
            clipper.Begin((__int32)values.size());

            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                    auto iter = values.begin();
                    std::advance(iter, i);
                    auto key = iter->first;
                    auto val = iter->second;

                    std::string pushid = lbl + val + "lcmbo";
                    ImGui::PushID(pushid.c_str());
                    const bool item_selected = (key == idx);
                    if (translate_items) {
                        val = localize.Translate(val.c_str());
                    }

                    const char* item_text = val.c_str();
                    if (ImGui::Selectable(item_text, item_selected, 0, ImVec2(245.0f, 0.0f)))
                    {
                        value_changed = true;
                        *currIndex = key;
                    }
                    if (item_selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::IsItemHovered()) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
            ImGui::SetTooltip(preview_value);
            ImGui::PopStyleVar();
        }

        ImGui::PopItemWidth();
        ImGui::EndChild();
        ImGui::PopID();
        ImGui::EndGroup();
        ImGui::PopStyleVar();

        return value_changed;
    }

    inline void AddUnderLine(ImColor col_)
    {
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        min.y = max.y;
        ImGui::GetWindowDrawList()->AddLine(min, max, col_, 1.0f);
    }

    // hyperlink urls
    inline void TextURL(const char* name_, const char* URL_, uint8_t SameLineBefore_, uint8_t SameLineAfter_)
    {
        if (1 == SameLineBefore_) { ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.x); }
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
        ImGui::Text(name_);
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered())
        {
            if (ImGui::IsMouseClicked(0))
            {
                ShellExecute(NULL, "open", URL_, NULL, NULL, SW_SHOWNORMAL);
            }
            AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
            ImGui::SetTooltip("Open in browser\n%s", URL_);
        }
        else
        {
            AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
        }
        if (1 == SameLineAfter_) { ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.x); }
    }

    inline void TextCenter(std::string text, std::string url = "", std::string url_address = "") {
        std::string full_txt = text + url;
        float txt_center = ImGui::CalcTextSize(full_txt.c_str()).x / 2;
        float window_center = ImGui::GetWindowWidth() / 2;
        ImGui::SetCursorPosX(window_center - txt_center);
        ImGui::Text(text.c_str());

        if (!url.empty()) {
            TextURL(url.c_str(), url_address.c_str(), 1, 1);
            ImGui::NewLine();
        }
    }

    // menu item urls
    inline void MenuItemURL(const char* name_, const char* URL_)
    {
        if (ImGui::MenuItem(name_))
        {
            ShellExecute(NULL, "open", URL_, NULL, NULL, SW_SHOWNORMAL);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Open in browser\n%s", URL_);
        }
    }

    inline void BasicTooltip(const char* key) {
        if (ImGui::IsItemHovered()) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
            ImGui::SetTooltip(localize.Translate(key).c_str());
            ImGui::PopStyleVar();
        }
    }
}

