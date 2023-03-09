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

