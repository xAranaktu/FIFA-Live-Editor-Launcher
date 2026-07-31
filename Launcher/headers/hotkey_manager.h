#pragma once

#include <windows.h>
#include <mutex>
#include <string>
#include <vector>
#include <map>

#include <config/config.h>

namespace LE {
    class HotkeyAction {
    public:
        HotkeyAction(std::string _id, std::string _name, std::vector<unsigned char> _keys);
        HotkeyAction(unsigned long _uid, std::string _name, std::vector<unsigned char> _keys);

        void SetKeys(std::vector<unsigned char> _keys);
        std::vector<unsigned char>* GetKeys();

        void SetValueType(LESetting::HotkeyValueType _val_type);
        LESetting::HotkeyValueType GetValueType();

        void SetFloat(float _fv);
        float* GetFloatValuePtr();

        unsigned long GetID();
        std::string GetName();
        std::string GetCombination();

        void SetDescription(std::string _description);
        std::string GetDescription();

    private:
        unsigned long uid; // DJB2
        std::string name;
        std::string description;
        std::string combination;
        std::vector<unsigned char> keys;
        LESetting::HotkeyValueType value_type;
        float fv;
    };

    class HotkeyManager {
    public:
        HotkeyManager(HotkeyManager& other) = delete;
        void operator=(const HotkeyManager&) = delete;
        static HotkeyManager* GetInstance();

        HotkeyAction* GetHotkeyAction(unsigned long id);
        std::vector<HotkeyAction*>* GetHotkeyActions();

    private:
        static HotkeyManager* pinstance_;
        static std::mutex mutex_;

        std::vector<HotkeyAction*> hotkey_actions;

        void CreateDefaultHotkeys();

    protected:
        HotkeyManager();
        ~HotkeyManager();
    };
}