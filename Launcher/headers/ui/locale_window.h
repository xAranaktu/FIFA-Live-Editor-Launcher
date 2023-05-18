#pragma once
#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "AES.h"

#include "core.h"

namespace UIWindows {
    class UILocaleIni
    {
    public:
        bool initialized = false;
        bool show = false;
        std::string window_name = "locale.ini";

        UILocaleIni();
        ~UILocaleIni();
        void Init();
        void Draw(bool* p_open);
        void Dock(ImGuiID dock_id);
        const char* GetWindowName();
    
    private:
        bool has_unsaved_changes = false;
        std::filesystem::path org_locale_path;
        std::filesystem::path locale_file;
        std::filesystem::path locale_backup;
        std::string file_content;

        // 0x20
        std::vector<unsigned char> key;
        // 0x10
        std::vector<unsigned char> iv;

        void MenuBar();
        void Content();

        void WriteEncryptedFileContent(std::filesystem::path f, std::vector<uint8_t> data);
        void SaveFileContent();

        void LoadLocale();
        void LoadFileContent(std::vector<unsigned char> data);
        std::vector<unsigned char> DecryptLocaleFile(std::vector<uint8_t>* buff);
        std::vector<unsigned char> EncryptLocaleFile(std::vector<uint8_t>* buff);

        void RestoreOrgLocale();

        bool IsKeyLoaded();
        void LoadKey();
    };
}