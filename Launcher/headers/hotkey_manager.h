#pragma once

#include <windows.h>
#include <mutex>
#include <string>
#include <vector>
#include <map>

#include <config.h>

namespace LE {
    enum class HotkeyActionID {
        ACTION_SHOW_UI = 0
    };

    class HotkeyAction {
    public:
        HotkeyAction(HotkeyActionID _id, std::string _name, std::vector<unsigned char> _keys);

        void SetKeys(std::vector<unsigned char> _keys);
        std::vector<unsigned char>* GetKeys();

        HotkeyActionID GetID();
        std::string GetName();
        std::string GetCombination();

        void SetDescription(std::string _description);
        std::string GetDescription();

    private:
        HotkeyActionID id;
        std::string name;
        std::string description;
        std::string combination;
        std::vector<unsigned char> keys;
    };

    class HotkeyManager {
    public:
        HotkeyManager(HotkeyManager& other) = delete;
        void operator=(const HotkeyManager&) = delete;
        static HotkeyManager* GetInstance();

        HotkeyAction* GetHotkeyAction(HotkeyActionID id);

    private:
        static HotkeyManager* pinstance_;
        static std::mutex mutex_;

        std::map<HotkeyActionID, HotkeyAction*> hotkey_actions;

    protected:
        HotkeyManager();
        ~HotkeyManager();
    };
}