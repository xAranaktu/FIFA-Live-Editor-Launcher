#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"

#include <../external/nlohmann/json.hpp>
using json = nlohmann::json;
namespace LESetting {
    enum class HotkeyValueType {
        HOTKEY_NO_VALUE = 0,
        HOTKEY_LUA_SCRIPT,
        HOTKEY_FLOAT,
        HOTKEY_INT,
        HOTKEY_STR
    };

    class Color3 {
    public:
        float x, y, z;
        Color3() { x = y = z = 0.0f; }
        Color3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }

        void to_json(json& j);
        void from_json(const json& j);

        ImU32 GetImU32() {
            ImU32 out;
            out = ((ImU32)IM_F32_TO_INT8_SAT(x)) << IM_COL32_R_SHIFT;
            out |= ((ImU32)IM_F32_TO_INT8_SAT(y)) << IM_COL32_G_SHIFT;
            out |= ((ImU32)IM_F32_TO_INT8_SAT(z)) << IM_COL32_B_SHIFT;
            out |= ((ImU32)IM_F32_TO_INT8_SAT(1.0f)) << IM_COL32_A_SHIFT;
            return out;
        }
    };

    class Attribute {
    public:
        Color3 bg_color;
        Color3 txt_color;
        int threshold;

        Attribute(int _threshold, Color3 _bg, Color3 _txt);

        void to_json(json& j);
        void from_json(const json& j);
    };

    class Hotkey {
    public:
        unsigned long uid;    // DJB2hash
        std::vector<int> keys_combination;
        std::string name;
        std::string description;
        HotkeyValueType val_type;
        float fv;
        std::string sv;

        Hotkey();
        Hotkey(unsigned long _uid, std::vector<int> combination_arr);
        void SetCombination(std::vector<int> combination_arr);
        void SetCombination(std::vector<uint8_t> combination_arr);
        void SetName(std::string _name);
        void SetDescription(std::string _description);
        void SetFloatValue(float _fv);
        void SetStringValue(std::string _sv);
        void SetLUAValue(std::string _sv);
        void SetValueType(HotkeyValueType _val_type);

        void to_json(json& j);
        void from_json(const json& j);
    };
}
