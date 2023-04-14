#pragma once
#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace ImGui {
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
}

