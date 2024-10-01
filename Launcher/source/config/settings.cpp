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

    Hotkey::Hotkey(std::vector<int> combination_arr) {
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

    void Hotkey::to_json(json& j) {
        j = json{
            {"enabled",     enabled},
            {"keys_combination",   keys_combination}
        };
    }
    void Hotkey::from_json(const json& j) {
        if (j.contains("enabled")) {
            j.at("enabled").get_to(enabled);
        }

        if (j.contains("keys_combination")) {
            j.at("keys_combination").get_to(keys_combination);
        }
    }
}
