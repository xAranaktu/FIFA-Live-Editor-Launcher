#pragma once
#include <string>
#include <vector>

#include <imgui.h>
#include <imgui_stdlib.h>
#include "localize.h"

namespace ImGuiLoc {
    bool BeginMenu(const char* key, bool enabled = true);
    bool BeginTabItem(const char* key, bool* p_open = NULL, ImGuiTabItemFlags flags = 0);
    bool MenuItem(const char* key, const char* shortcut = NULL, bool selected = false, bool enabled = true);
    bool Button(const char* key, const ImVec2& size = ImVec2(0, 0));
    bool Checkbox(const char* key, bool* v);
    bool InputInt(const char* key, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
    bool Combo(const char* key, int* currIndex, std::vector<std::string>& values);
    bool CollapsingHeader(const char* key, ImGuiTreeNodeFlags flags = 0);
    bool ColorEdit3(const char* key, float col[3], ImGuiColorEditFlags flags = 0);
    bool ColorEdit4(const char* key, float col[4], ImGuiColorEditFlags flags = 0);

    void TextCenter(std::string key);
}