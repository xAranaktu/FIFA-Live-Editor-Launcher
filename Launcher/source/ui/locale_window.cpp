#include "locale_window.h"

namespace UIWindows {
    UILocaleIni::UILocaleIni() {
        text_editor.SetImGuiChildIgnored(true);
        text_editor.SetColorizerEnable(true);
    }
    UILocaleIni::~UILocaleIni() {}

    void UILocaleIni::Init() {
        if (initialized)    return;

        text_editor.SetText("locale.ini is encrypted. Live Editor will decrypt it once you successfuly run the game with this tool\nTHIS IS NOT A BUG");

        LoadKey();

        LE::FilesManager* files_manager = LE::FilesManager::GetInstance();
        org_locale_path = files_manager->GetGameDirectory() / "Data" / "locale.ini";
        if (!fs::exists(org_locale_path)) return;

        locale_file = files_manager->GetLEModsDirectory() / "locale.ini";
        locale_backup = files_manager->GetLEModsDirectory() / "locale_backup.ini";

        if (!fs::exists(locale_backup)) {
            fs::copy_file(org_locale_path, locale_backup);
        }

        if (!fs::exists(locale_file)) {
            fs::copy_file(org_locale_path, locale_file);
        }

        LoadLocale();

        initialized = true;
    }

    void UILocaleIni::Draw(bool* p_open) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
        if (has_unsaved_changes) {
            window_flags |= ImGuiWindowFlags_UnsavedDocument;
        }

        if (ImGui::Begin(GetWindowName(), p_open, window_flags)) {
            MenuBar();
            Content();
        }

        ImGui::End();
    }

    void UILocaleIni::Dock(ImGuiID dock_id) {
        ImGui::DockBuilderDockWindow(GetWindowName(), dock_id);
    }

    const char* UILocaleIni::GetWindowName() {
        return window_name.c_str();
    }

    void UILocaleIni::MenuBar() {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save")) {
                    SaveFileContent();
                }

                if (ImGui::MenuItem("Restore Original")) {
                    RestoreOrgLocale();
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Restore original locale.ini file.");
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void UILocaleIni::Content() {
        text_editor.Render("Locale Editor", ImVec2(0, 0));
    }

    void UILocaleIni::WriteEncryptedFileContent(std::filesystem::path f, std::vector<uint8_t> data) {
        LOG_INFO(std::format("[{}] {}", __FUNCTION__, ToUTF8String(f)));

        std::ofstream _stream(f, std::ios::out | std::ios::binary);
        if (!_stream) {
            LOG_WARN(std::format("[{}] Can't open {}", __FUNCTION__, ToUTF8String(locale_file).c_str()));
            return;
        }
        std::copy(data.cbegin(), data.cend(), std::ostream_iterator<unsigned char>(_stream));

        _stream.close();

        LOG_INFO(std::format("[{}] Done", __FUNCTION__));
    }

    void UILocaleIni::SaveFileContent() {
        if (!IsKeyLoaded()) return;

        LOG_INFO(std::format("[{}]", __FUNCTION__));

        std::string file_content = text_editor.GetText();
        std::vector<uint8_t> buf(file_content.begin(), file_content.end());

        auto encrypted = EncryptLocaleFile(&buf);

        WriteEncryptedFileContent(locale_file, encrypted);
        WriteEncryptedFileContent(org_locale_path, encrypted);

        has_unsaved_changes = false;
        LOG_INFO(std::format("[{}] Done", __FUNCTION__));
    }

    void UILocaleIni::LoadLocale() {
        LOG_INFO(std::format("[{}]", __FUNCTION__));
        if (fs::exists(locale_file) && IsKeyLoaded()) {
            LOG_INFO(std::format("[{}] Load Encrypted {}", __FUNCTION__, ToUTF8String(locale_file).c_str()));

            std::ifstream encstream(locale_file, std::ios::in | std::ios::binary);
            std::vector<uint8_t> enc_buffer((std::istreambuf_iterator<char>(encstream)), std::istreambuf_iterator<char>());

            auto decrypted = DecryptLocaleFile(&enc_buffer);
            LoadFileContent(decrypted);

            encstream.close();
        }
        LOG_INFO(std::format("[{}] Done", __FUNCTION__));
    }

    void UILocaleIni::LoadFileContent(std::vector<unsigned char> data) {
        text_editor.SetText(std::string(data.begin(), data.end()));
    }

    std::vector<unsigned char> UILocaleIni::DecryptLocaleFile(std::vector<uint8_t>* buff) {
        LOG_INFO(std::format("[{}]", __FUNCTION__));
        std::vector<unsigned char> result;
        
        try
        {
            AES aes(AESKeyLength::AES_256);
            result = aes.DecryptCBC(*buff, key, iv);
        }
        catch (const std::exception& e)
        {
            LOG_ERROR(std::format("DecryptLocaleFile err {}", e.what()));
        }
        LOG_INFO(std::format("[{}] Done", __FUNCTION__));
        return result;
    }

    std::vector<unsigned char> UILocaleIni::EncryptLocaleFile(std::vector<uint8_t>* buff) {
        LOG_INFO(std::format("[{}]", __FUNCTION__));
        std::vector<unsigned char> result;

        // nullbyte fill
        if (buff->size() % 16 != 0) {
            int i = (16 - buff->size() % 16);
            while (i > 0)
            {
                buff->push_back(0x00);
                i--;
            }
        }

        try
        {
            AES aes(AESKeyLength::AES_256);
            result = aes.EncryptCBC(*buff, key, iv);
        }
        catch (const std::exception& e)
        {
            LOG_ERROR(std::format("EncryptLocaleFile err {}", e.what()));
        }
        
        LOG_INFO(std::format("[{}] Done - {}", __FUNCTION__, result.size()));

        return result;
    }

    void UILocaleIni::RestoreOrgLocale() {
        LOG_INFO(std::format("[{}]", __FUNCTION__));
        try {
            if (fs::exists(org_locale_path)) {
                LOG_INFO(std::format("[{}] remove {}", __FUNCTION__, ToUTF8String(org_locale_path).c_str()));
                fs::remove(org_locale_path);
            }

            if (!fs::exists(org_locale_path)) {
                LOG_INFO(std::format("[{}] copy {} -> {}", __FUNCTION__, ToUTF8String(locale_backup).c_str(), ToUTF8String(org_locale_path).c_str()));
                fs::copy_file(locale_backup, org_locale_path);
            }

            if (fs::exists(locale_file)) {
                LOG_INFO(std::format("[{}] remove {}", __FUNCTION__, ToUTF8String(locale_file).c_str()));
                fs::remove(locale_file);
            }

            if (!fs::exists(locale_file)) {
                LOG_INFO(std::format("[{}] copy {} -> {}", __FUNCTION__, ToUTF8String(locale_backup).c_str(), ToUTF8String(locale_file).c_str()));
                fs::copy_file(locale_backup, locale_file);
            }
        } 
        catch (const std::exception& e)
        {
            LOG_ERROR(std::format("[{}] err {}", __FUNCTION__, e.what()));
        }

        LoadLocale();
        LOG_INFO(std::format("[{}] Done", __FUNCTION__));
    }

    bool UILocaleIni::IsKeyLoaded() {
        return key.size() > 0 && iv.size() > 0;
    }

    void UILocaleIni::LoadKey() {
        std::filesystem::path keyfile_path = LE::FilesManager::GetInstance()->GetLocaleKeyPath();
        if (!fs::exists(keyfile_path)) {
            return;
        }

        auto fsize = std::filesystem::file_size(keyfile_path);
        if (fsize != 48) {
            LOG_ERROR(std::format("[{}] Invalid key file size {} != 48", __FUNCTION__, fsize));
            return;
        }

        char keybuf[0x20];
        char ivbuf[0x10];

        std::ifstream keystream(keyfile_path, std::ios::in | std::ios::binary);
        if (!keystream) {
            LOG_ERROR(std::format("[{}] Can't open {}", __FUNCTION__, ToUTF8String(keyfile_path).c_str()));
            return;
        }

        keystream.read(keybuf,  0x20);
        keystream.read(ivbuf,   0x10);

        for (int i = 0; i < 0x20; i++) {
            unsigned char b = static_cast<unsigned char>(keybuf[i]);
            key.push_back(b);
        }

        for (int i = 0; i < 0x10; i++) {
            unsigned char b = static_cast<unsigned char>(ivbuf[i]);
            iv.push_back(b);
        }

        keystream.close();
        show = true;
    }

}