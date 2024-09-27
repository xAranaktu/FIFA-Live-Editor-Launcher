#include <hotkey_manager.h>

namespace LE {
    HotkeyAction::HotkeyAction(HotkeyActionID _id, std::string _name, std::vector<unsigned char> _keys) {
        id = _id;
        name = _name;
        keys = _keys;
        combination.clear();
    }

    void HotkeyAction::SetKeys(std::vector<unsigned char> _keys) {
        keys = _keys;
        combination.clear();
    }

    std::vector<unsigned char>* HotkeyAction::GetKeys() { 
        return &keys; 
    }

    HotkeyActionID HotkeyAction::GetID() {
        return id;
    }

    std::string HotkeyAction::GetName() {
        return name;
    }

    std::string HotkeyAction::GetCombination() {
        if (!combination.empty()) return combination;

        char buffer[256];
        for (auto code : keys) {
            if (code <= 0)  continue;
            if (code >= 256) continue;

            GetKeyNameTextA(MapVirtualKeyW(code, MAPVK_VK_TO_VSC) << 16, buffer, 256);
            combination += std::string(buffer) + " + ";
        }
        combination = combination.substr(0, combination.size() - 3);

        return combination;
    }

    void HotkeyAction::SetDescription(std::string _description) {
        description = _description;
    }

    std::string HotkeyAction::GetDescription() {
        return description;
    }

    HotkeyManager::HotkeyManager() {
        LE::HotkeysValues* hotkey_values = LE::Config::GetInstance()->GetHotkeyValues();
        
        // Show UI
        {
            std::vector<unsigned char> tmpkeys;
            for (auto key : hotkey_values->show_ui_keys.keys_combination) {
                tmpkeys.push_back(key);
            }
            HotkeyAction* show_ui = new HotkeyAction(HotkeyActionID::ACTION_SHOW_UI, "Show UI", tmpkeys);
            show_ui->SetDescription("Show/hide live editor user interface in-game");
            hotkey_actions[HotkeyActionID::ACTION_SHOW_UI] = show_ui;
        }
    }
    HotkeyManager::~HotkeyManager() {}

    HotkeyAction* HotkeyManager::GetHotkeyAction(HotkeyActionID id) {
        return hotkey_actions[id];
    }

    HotkeyManager* HotkeyManager::GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
            pinstance_ = new HotkeyManager();

        return pinstance_;
    }
}

LE::HotkeyManager* LE::HotkeyManager::pinstance_{ nullptr };
std::mutex LE::HotkeyManager::mutex_;