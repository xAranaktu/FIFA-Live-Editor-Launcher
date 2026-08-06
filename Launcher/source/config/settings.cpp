#include "config/settings.h"

namespace LESetting {
    void Color3::to_json(json& j) {
        j = json{
            {"r", x},
            {"g", y},
            {"b", z}
        };
    }

    void Color3::from_json(const json& j) {
        if (j.contains("r")) {
            j.at("r").get_to(x);
        }

        if (j.contains("g")) {
            j.at("g").get_to(y);
        }

        if (j.contains("b")) {
            j.at("b").get_to(z);
        }
    }

    Attribute::Attribute(int _threshold, Color3 _bg, Color3 _txt) {
        threshold = _threshold;
        bg_color = _bg;
        txt_color = _txt;
    }

    void Attribute::to_json(json& j) {
        json obj_bg_color = json::object();
        json obj_txt_color = json::object();

        bg_color.to_json(obj_bg_color);
        txt_color.to_json(obj_txt_color);

        j = json{
            {"bg_color",    obj_bg_color},
            {"txt_color",   obj_txt_color},
            {"threshold",   threshold}
        };
    }

    void Attribute::from_json(const json& j) {
        if (j.contains("bg_color")) {
            bg_color.from_json(j.at("bg_color"));
        }

        if (j.contains("txt_color")) {
            txt_color.from_json(j.at("txt_color"));
        }

        if (j.contains("threshold")) {
            j.at("threshold").get_to(threshold);
        }
    }

    Hotkey::Hotkey() {
        val_type = HotkeyValueType::HOTKEY_NO_VALUE;
        uid = -1;
        fv = 1.0f;
        sv = "";
        keys_combination.clear();
        name.clear();
        description.clear();
    }

    Hotkey::Hotkey(unsigned long _uid, std::vector<int> combination_arr) {
        val_type = HotkeyValueType::HOTKEY_NO_VALUE;
        fv = 1.0f;
        sv = "";
        keys_combination.clear();
        name.clear();
        description.clear();

        uid = _uid;
        SetCombination(combination_arr);
    }

    void Hotkey::SetCombination(std::vector<int> combination_arr) {
        keys_combination = combination_arr;
    }

    void Hotkey::SetCombination(std::vector<uint8_t> combination_arr) {
        keys_combination.clear();
        for (auto key : combination_arr) {
            keys_combination.push_back(key);
        }
    }

    void Hotkey::SetName(std::string _name) {
        name = _name;
    }
    
    void Hotkey::SetDescription(std::string _description) {
        description = _description;
    }

    void Hotkey::SetFloatValue(float _fv) {
        fv = _fv;
        SetValueType(HotkeyValueType::HOTKEY_FLOAT);
    }

    void Hotkey::SetLUAValue(std::string _sv) {
        sv = _sv;
        SetValueType(HotkeyValueType::HOTKEY_LUA_SCRIPT);
    }

    void Hotkey::SetStringValue(std::string _sv) {
        sv = _sv;
        SetValueType(HotkeyValueType::HOTKEY_STR);
    }
    
    void Hotkey::SetValueType(HotkeyValueType _val_type) {
        val_type = _val_type;
    }

    void Hotkey::to_json(json& j) {
        j = json{
            {"uid",                 uid},
            {"val_type",            val_type},
            {"name",                name},
            {"description",         description},
            {"fv",                  fv},
            {"sv",                  sv},
            {"keys_combination",    keys_combination}
        };
    }

    void Hotkey::from_json(const json& j) {
        if (j.contains("uid")) {
            j.at("uid").get_to(uid);
        }

        if (j.contains("val_type")) {
            j.at("val_type").get_to(val_type);
        }

        if (j.contains("name")) {
            j.at("name").get_to(name);
        }

        if (j.contains("description")) {
            j.at("description").get_to(description);
        }

        if (j.contains("fv")) {
            j.at("fv").get_to(fv);
        }

        if (j.contains("sv")) {
            j.at("sv").get_to(sv);
        }

        if (j.contains("keys_combination")) {
            j.at("keys_combination").get_to(keys_combination);
        }
    }
}
