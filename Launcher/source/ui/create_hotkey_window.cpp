#include <ui/create_hotkey_window.h>

namespace LE {
    CreateHotkeyWindow::CreateHotkeyWindow() {
        default_path = ".";
        input_last_change = std::chrono::system_clock::now();
        error.clear();

        avail_action_types = {
            "Execute LUA"
        };

        avail_key_names = {
            { VK_LSHIFT, "LEFT SHIFT" },
            { VK_RSHIFT, "RIGHT SHIFT" },
            { VK_LCONTROL, "LEFT CTRL" },
            { VK_RCONTROL, "RIGHT CTRL" },
            { VK_LMENU, "LEFT ALT" },
            { VK_RMENU, "RIGHT ALT" },
            { VK_PRIOR, "PAGE UP" },
            { VK_NEXT, "PAGE DOWN" },
            { VK_END, "END" },
            { VK_HOME, "HOME" },
            { VK_INSERT, "INS" },
            { VK_DELETE, "DEL" },
            { 0x30, "0" },
            { 0x31, "1" },
            { 0x32, "2" },
            { 0x33, "3" },
            { 0x34, "4" },
            { 0x35, "5" },
            { 0x36, "6" },
            { 0x37, "7" },
            { 0x38, "8" },
            { 0x39, "9" },
            { 0x41, "A" },
            { 0x42, "B" },
            { 0x43, "C" },
            { 0x44, "D" },
            { 0x45, "E" },
            { 0x46, "F" },
            { 0x47, "G" },
            { 0x48, "H" },
            { 0x49, "I" },
            { 0x4A, "J" },
            { 0x4B, "K" },
            { 0x4C, "L" },
            { 0x4D, "M" },
            { 0x4E, "N" },
            { 0x4F, "O" },
            { 0x50, "P" },
            { 0x51, "Q" },
            { 0x52, "R" },
            { 0x53, "S" },
            { 0x54, "T" },
            { 0x55, "U" },
            { 0x56, "V" },
            { 0x57, "W" },
            { 0x58, "X" },
            { 0x59, "Y" },
            { 0x5A, "Z" },
            { VK_NUMPAD0, "NUMPAD0" },
            { VK_NUMPAD1, "NUMPAD1" },
            { VK_NUMPAD2, "NUMPAD2" },
            { VK_NUMPAD3, "NUMPAD3" },
            { VK_NUMPAD4, "NUMPAD4" },
            { VK_NUMPAD5, "NUMPAD5" },
            { VK_NUMPAD6, "NUMPAD6" },
            { VK_NUMPAD7, "NUMPAD7" },
            { VK_NUMPAD8, "NUMPAD8" },
            { VK_NUMPAD9, "NUMPAD9" },
            { VK_F1, "F1" },
            { VK_F2, "F2" },
            { VK_F3, "F3" },
            { VK_F4, "F4" },
            { VK_F5, "F5" },
            { VK_F6, "F6" },
            { VK_F7, "F7" },
            { VK_F8, "F8" },
            { VK_F9, "F9" },
            { VK_F10, "F10" },
            { VK_F11, "F11" },
            { VK_F12, "F12" }
        };

        avail_keys = {
            "No key",
            "LEFT SHIFT", "RIGHT SHIFT",
            "LEFT CTRL", "RIGHT CTRL",
            "LEFT ALT", "RIGHT ALT",
            "PAGE UP",
            "PAGE DOWN",
            "END",
            "HOME",
            "INS",
            "DEL",
            "0", "1", "2", "3", "4", "5", "6","7","8","9",
            "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
            "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
            "NUMPAD0", "NUMPAD1", "NUMPAD2", "NUMPAD3", "NUMPAD4", "NUMPAD5", "NUMPAD6", "NUMPAD7", "NUMPAD8", "NUMPAD9",
            "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12"
        };
    }
    CreateHotkeyWindow::~CreateHotkeyWindow() {}

    void CreateHotkeyWindow::Open() {
        keys.clear();
        name.clear();
        description.clear();
        error.clear();
        lua_script.clear();
        show = true;
    }

    void CreateHotkeyWindow::Draw() {
        if (!show)  return;

        auto viewport = ImGui::GetMainViewport();
        ImVec2 center = viewport->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        // Half Size
        ImVec2 sz = viewport->Size;
        sz.x *= 0.5f;
        sz.y *= 0.5f;
        ImGui::SetNextWindowSize(sz, ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Create Hotkey", &show)) {
            auto avail = ImGui::GetContentRegionAvail();

            ImGui::Text("Hotkey: %s", current_action.GetName().c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
                ImGui::SetTooltip(current_action.GetDescription().c_str());
                ImGui::PopStyleVar();
            }

            ImGui::Text("Current Combination: %s", new_combination.c_str());
            ImGui::Separator();

            ImGui::Text("Action Type:   ");
            ImGui::SameLine();

            ImGui::PushItemWidth(200.0f);
            if (ImGui::Combo("##hotkey_action_type", &current_hotkey_action_type, avail_action_types)) {

            }
            ImGui::PopItemWidth();

            ImGui::Text("Script:        ");
            ImGui::SameLine();
            ImGui::PushID("##LUASCRIPTFDBTN");
            if (ImGui::Button("...")) {
                CloseCurrentFileDialog();

                IGFD::FileDialogConfig cfg;
                cfg.path = default_path;
                ImGuiFileDialog::Instance()->OpenDialog("LUAFD", "Choose a LUA File", "Text files (*.txt *.lua){.txt,.lua}", cfg);
            }
            ImGui::PopID();
            ImGui::SameLine();

            ImGui::Text(lua_script.c_str());

            ImGui::Text("Name:          "); ImGui::SameLine();
            if (ImGui::InputText("##hotkey_name", &name)) {
                text_input_changed = true;
                input_last_change = std::chrono::system_clock::now();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
                ImGui::SetTooltip("Hotkey Unique Name");
                ImGui::PopStyleVar();
            }

            ImGui::Text("Description:   "); ImGui::SameLine();
            if (ImGui::InputText("##hotkey_description", &description)) {

            }
            if (ImGui::IsItemHovered()) {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
                ImGui::SetTooltip("Hotkey Description");
                ImGui::PopStyleVar();
            }

            for (size_t i = 0; i < keys.size(); i++)
            {
                ImGui::Text("Key %d: ", i + 1);
                ImGui::SameLine();

                ImGui::PushItemWidth(200.0f);
                if (ImGui::Combo(std::format("##key{}", i).c_str(), (int*)&keys[i], avail_keys)) {
                    UpdateCombination();
                }
                ImGui::PopItemWidth();
            }

            if (ImGui::Button("Add key")) {
                keys.push_back(0);
            }
            if (ImGui::Button("Reset all")) {
                keys.clear();
                keys.push_back(0);
                new_combination = "No key";
            }

            auto cps = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(cps.x, avail.y - 16));
            ImGui::Separator();

            if (!error.empty()) {
                ImGui::Text(error.c_str());
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Save", ImVec2(-FLT_MIN, 0.0f))) {
                SaveHotkey();
            }

            if (!error.empty()) {
                ImGui::EndDisabled();
            }
        }
        ImGui::End();

        if (
            text_input_changed &&
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - input_last_change).count() > 500
        ) {
            auto uid = DJB2hash(name);
            bool is_unique = true;

            LE::HotkeyManager* hotkey_manager = LE::HotkeyManager::GetInstance();
            for (auto hotkey : *hotkey_manager->GetHotkeyActions()) {
                if (hotkey->GetID() == uid) {
                    is_unique = false;
                    break;
                }
            }

            if (is_unique) {
                current_action.SetID(uid);
                current_action.SetName(name);
                error.clear();
            }
            else {
                error = "Hotkey name must be unique";
            }
        }
    }


    std::string CreateHotkeyWindow::GetKeyName(unsigned char code) {
        std::string result = std::format("KEY_{}", code);
        if (code <= 0 || code >= 256)  return result;

        char buffer[256];
        GetKeyNameTextA(MapVirtualKeyW(code, MAPVK_VK_TO_VSC) << 16, buffer, 256);
        result = std::string(buffer);
        return result;
    }

    void CreateHotkeyWindow::UpdateCombination() {
        new_combination.clear();
        for (int key : keys) {
            if (key == 0)  continue;
            new_combination += avail_keys[key] + " + ";
        }
        new_combination = new_combination.substr(0, new_combination.size() - 3);
    }

    void CreateHotkeyWindow::SetLUAScriptPath(std::filesystem::path fpath) {
        lua_script = ToUTF8String(fpath);
    }

    void CreateHotkeyWindow::SaveHotkey() {
        if (name.empty()) {
            error = "Hotkey name can't be empty";
            return;
        }

        std::vector<int> new_keys;
        for (int key : keys) {
            if (key == 0)  continue;

            std::string key_name = avail_keys[key];

            for (const auto& [code, name] : avail_key_names) {
                if (name == key_name) {
                    new_keys.push_back(code);
                    break;
                }
            }
        }

        LE::Config* config = LE::Config::GetInstance();

        if (description.empty())  description = "No Description";

        LESetting::Hotkey _hotkey = LESetting::Hotkey(DJB2hash(name), new_keys);
        _hotkey.SetName(name);
        _hotkey.SetDescription(description);
        _hotkey.SetLUAValue(lua_script);

        LE::HotkeyManager::GetInstance()->AddHotkeyToActions(&_hotkey);

        show = false;
    }

    void CreateHotkeyWindow::CloseCurrentFileDialog() {
        std::string opened_key = ImGuiFileDialog::Instance()->GetOpenedKey();

        if (opened_key.empty()) return;
        if (!ImGuiFileDialog::Instance()->IsOpened())   return;

        // close
        ImGuiFileDialog::Instance()->Close();
    }

    CreateHotkeyWindow* CreateHotkeyWindow::GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
            pinstance_ = new CreateHotkeyWindow();

        return pinstance_;
    }
}

LE::CreateHotkeyWindow* LE::CreateHotkeyWindow::pinstance_{ nullptr };
std::mutex LE::CreateHotkeyWindow::mutex_;