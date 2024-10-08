#include "ui/imgui_localize.h"

namespace ImGuiLoc {
    bool BeginMenu(const char* key, bool enabled) {
        return ImGui::BeginMenu(localize.Translate(key).c_str(), enabled);
    }

    bool BeginTabItem(const char* key, bool* p_open, ImGuiTabItemFlags flags) {
        return ImGui::BeginTabItem(localize.Translate(key).c_str(), p_open, flags);
    }

    bool MenuItem(const char* key, const char* shortcut, bool selected, bool enabled) {
        return ImGui::MenuItem(localize.Translate(key).c_str(), shortcut, selected, enabled);
    }

    bool Button(const char* key, const ImVec2& size) {
        return ImGui::Button(localize.Translate(key).c_str(), size);
    }

    bool Checkbox(const char* key, bool* v) {
        return ImGui::Checkbox(localize.Translate(key).c_str(), v);
    }

    bool InputInt(const char* key, int* v, int step, int step_fast, ImGuiInputTextFlags flags) {
        return ImGui::InputInt(localize.Translate(key).c_str(), v, step, step_fast, flags);
    }

    bool Combo(const char* key, int* currIndex, std::vector<std::string>& values) {
        return ImGui::Combo(localize.Translate(key).c_str(), currIndex, values);
    }

    bool CollapsingHeader(const char* key, ImGuiTreeNodeFlags flags) {
        return ImGui::CollapsingHeader(localize.Translate(key).c_str(), flags);
    }

    bool ColorEdit3(const char* key, float col[3], ImGuiColorEditFlags flags) {
        return ImGui::ColorEdit3(localize.Translate(key).c_str(), col, flags);
    }

    bool ColorEdit4(const char* key, float col[4], ImGuiColorEditFlags flags) {
        return ImGui::ColorEdit4(localize.Translate(key).c_str(), col, flags);
    }

    void TextCenter(std::string key) {
        std::string text = localize.Translate(key);
        float txt_center = ImGui::CalcTextSize(text.c_str()).x / 2;
        float window_center = ImGui::GetWindowWidth() / 2;
        ImGui::SetCursorPosX(window_center - txt_center);
        ImGui::Text(text.c_str());
    }
}