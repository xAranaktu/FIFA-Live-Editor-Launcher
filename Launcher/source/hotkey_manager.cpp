#include <hotkey_manager.h>

namespace LE {
    HotkeyAction::HotkeyAction() {
        uid = 0;
        name.clear();
        keys.clear();
        combination.clear();
        value_type = LESetting::HotkeyValueType::HOTKEY_NO_VALUE;
        fv = 0.0f;
    }

    HotkeyAction::HotkeyAction(std::string _id, std::string _name, std::vector<unsigned char> _keys) {
        uid = DJB2hash(_id);
        name = _name;
        keys = _keys;
        combination.clear();
        value_type = LESetting::HotkeyValueType::HOTKEY_NO_VALUE;
        fv = 0.0f;
    }

    HotkeyAction::HotkeyAction(unsigned long _uid, std::string _name, std::vector<unsigned char> _keys) {
        uid = _uid;
        name = _name;
        keys = _keys;
        combination.clear();
        value_type = LESetting::HotkeyValueType::HOTKEY_NO_VALUE;
        fv = 0.0f;
    }

    void HotkeyAction::SetKeys(std::vector<unsigned char> _keys) {
        keys = _keys;
        combination.clear();
    }

    std::vector<unsigned char>* HotkeyAction::GetKeys() { 
        return &keys; 
    }

    void HotkeyAction::SetValueType(LESetting::HotkeyValueType _val_type) {
        value_type = _val_type;
    }

    LESetting::HotkeyValueType HotkeyAction::GetValueType() {
        return value_type;
    }

    void HotkeyAction::SetFloat(float _fv) {
        SetValueType(LESetting::HotkeyValueType::HOTKEY_FLOAT);
        fv = _fv;
    }

    void HotkeyAction::SetLUAValue(std::string _sv) {
        SetValueType(LESetting::HotkeyValueType::HOTKEY_LUA_SCRIPT);
        sv = _sv;
    }

    std::string HotkeyAction::GetStringValue() {
        return sv;
    }

    float* HotkeyAction::GetFloatValuePtr() {
        return &fv;
    }

    void HotkeyAction::SetID(unsigned long _id) {
        uid = _id;
    }

    unsigned long HotkeyAction::GetID() {
        return uid;
    }

    void HotkeyAction::SetName(std::string _name) {
        name = _name;
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
        CreateDefaultHotkeys();

        for (auto hotkey : LE::Config::GetInstance()->GetHotkeyValues()->hotkeys) {
            std::vector<unsigned char> tmpkeys;
            for (auto key : hotkey.keys_combination) {
                tmpkeys.push_back(key);
            }
            HotkeyAction* action = new HotkeyAction(hotkey.uid, hotkey.name, tmpkeys);
            action->SetDescription(hotkey.description);

            if (hotkey.val_type == LESetting::HotkeyValueType::HOTKEY_FLOAT) {
                action->SetFloat(hotkey.fv);
            }
            else if (hotkey.val_type == LESetting::HotkeyValueType::HOTKEY_LUA_SCRIPT) {
                action->SetLUAValue(hotkey.sv);
            }

            hotkey_actions.push_back(action);
        }
    }
    HotkeyManager::~HotkeyManager() {}

    HotkeyAction* HotkeyManager::GetHotkeyAction(unsigned long id) {
        for (auto& hotkey : hotkey_actions) {
            if (hotkey->GetID() == id)  return hotkey;
        }

        return nullptr;
    }

    std::vector<HotkeyAction*>* HotkeyManager::GetHotkeyActions() {
        return &hotkey_actions;
    }

    void HotkeyManager::AddHotkeyToActions(LESetting::Hotkey* hotkey) {
        std::vector<unsigned char> tmpkeys;
        for (auto key : hotkey->keys_combination) {
            tmpkeys.push_back(key);
        }
        HotkeyAction* action = new HotkeyAction(hotkey->uid, hotkey->name, tmpkeys);
        action->SetDescription(hotkey->description);
        if (hotkey->val_type == LESetting::HotkeyValueType::HOTKEY_FLOAT) {
            action->SetFloat(hotkey->fv);
        }
        else if (hotkey->val_type == LESetting::HotkeyValueType::HOTKEY_LUA_SCRIPT) {
            action->SetLUAValue(hotkey->sv);
        }
        hotkey_actions.push_back(action);

        LE::Config* config = LE::Config::GetInstance();
        config->GetHotkeyValues()->AddHotkey(*hotkey);
        config->Save();
    }

    void HotkeyManager::DeleteHotkey(unsigned long id) {
        for (auto it = hotkey_actions.begin(); it != hotkey_actions.end(); ++it) {
            if ((*it)->GetID() == id) {
                delete* it;
                hotkey_actions.erase(it);
                break;
            }
        }
        LE::Config* config = LE::Config::GetInstance();
        config->GetHotkeyValues()->DeleteHotkey(id);
        config->Save();
    }

    void HotkeyManager::CreateDefaultHotkeys() {
        LE::HotkeysValues* hotkey_values = LE::Config::GetInstance()->GetHotkeyValues();
        // Gameplay Speed
        for (size_t i = 0; i < 3; i++)
        {
            auto _hash = DJB2hash(std::format("GAMEPLAY_SPEED##{}", i));
            if (hotkey_values->HotkeyExist(_hash)) continue;

            std::vector<int> _keys;
            LESetting::Hotkey _hotkey = LESetting::Hotkey(_hash, _keys);
            _hotkey.SetName(std::format("Gameplay Speed {}", i + 1));
            _hotkey.SetDescription("Set Speedhack Gameplay Speed");
            _hotkey.SetFloatValue(1.0f);

            hotkey_values->AddHotkey(_hotkey);
        }

        {
            auto _hash = DJB2hash("GAMEPLAY_INC_SPEED##0");
            if (!hotkey_values->HotkeyExist(_hash)) {
                std::vector<int> _keys;
                LESetting::Hotkey _hotkey = LESetting::Hotkey(_hash, _keys);
                _hotkey.SetName("Gameplay Speed +");
                _hotkey.SetDescription("Increase Speedhack Speed During Gameplay");
                _hotkey.SetFloatValue(1.0f);
                hotkey_values->AddHotkey(_hotkey);
            }
        }

        {
            auto _hash = DJB2hash("GAMEPLAY_DEC_SPEED##0");
            if (!hotkey_values->HotkeyExist(_hash)) {
                std::vector<int> _keys;
                LESetting::Hotkey _hotkey = LESetting::Hotkey(_hash, _keys);
                _hotkey.SetName("Gameplay Speed -");
                _hotkey.SetDescription("Decrease Speedhack Speed During Gameplay");
                _hotkey.SetFloatValue(1.0f);
                hotkey_values->AddHotkey(_hotkey);
            }
        }

        // Menu Speed
        for (size_t i = 0; i < 3; i++)
        {
            auto _hash = DJB2hash(std::format("MENU_SPEED##{}", i));
            if (hotkey_values->HotkeyExist(_hash)) continue;

            std::vector<int> _keys;
            LESetting::Hotkey _hotkey = LESetting::Hotkey(_hash, _keys);
            _hotkey.SetName(std::format("Menu Speed {}", i + 1));
            _hotkey.SetDescription("Set Speedhack Menu Speed");
            _hotkey.SetFloatValue(1.0f);
            hotkey_values->AddHotkey(_hotkey);
        }

        {
            auto _hash = DJB2hash("MENU_INC_SPEED##0");
            if (!hotkey_values->HotkeyExist(_hash)) {
                std::vector<int> _keys;
                LESetting::Hotkey _hotkey = LESetting::Hotkey(_hash, _keys);
                _hotkey.SetName("Menu Speed +");
                _hotkey.SetDescription("Increase Speedhack Speed In Menu");
                _hotkey.SetFloatValue(1.0f);

                hotkey_values->AddHotkey(_hotkey);
            }
        }

        {
            auto _hash = DJB2hash("MENU_DEC_SPEED##0");
            if (!hotkey_values->HotkeyExist(_hash)) {
                std::vector<int> _keys;
                LESetting::Hotkey _hotkey = LESetting::Hotkey(_hash, _keys);
                _hotkey.SetName("Menu Speed -");
                _hotkey.SetDescription("Decrease Speedhack Speed In Menu");
                _hotkey.SetFloatValue(1.0f);

                hotkey_values->AddHotkey(_hotkey);
            }
        }

        auto toggle_ui_hash = DJB2hash("TOGGLE_UI##0");
        if (!hotkey_values->HotkeyExist(toggle_ui_hash)) {
            LOG_INFO("Create Hotkey: Toggle UI - [F9]");
            std::vector<int> show_ui_keys;
            show_ui_keys.push_back(0x78); // F9

            LESetting::Hotkey show_ui_hotkey = LESetting::Hotkey(
                toggle_ui_hash, show_ui_keys
            );
            show_ui_hotkey.SetName("Toggle UI");
            show_ui_hotkey.SetDescription("Show/hide live editor user interface in-game");
            hotkey_values->AddHotkey(show_ui_hotkey);

            LE::Config::GetInstance()->Save();
        }
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