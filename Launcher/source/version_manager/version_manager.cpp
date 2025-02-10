#include <version_manager/version_manager.h>

namespace LE {
    VersionManager::VersionManager() {}
    VersionManager::~VersionManager() {}

    void VersionManager::Init() {
        LOG_INFO(std::format("[{}]", __FUNCTION__));

        game_version = GetLocalGameVersion();
        LOG_INFO(std::format("Game Version: {}", game_version.c_str()));

        ValidateDLL();

        std::thread t1(&LE::VersionManager::CheckUpdates, this);
        t1.detach();
    }

    void VersionManager::CheckUpdates() {
        LOG_FUNC_START();
        std::string version_url = "https://raw.githubusercontent.com/xAranaktu/FC-25-Live-Editor/refs/heads/main/version.json";
        std::string compatible_range_low;
        std::string compatible_range_high;

        cpr::Session session;
        session.SetHeader(cpr::Header{ { "User-Agent", std::format("FC Live Editor {}", tool_version) } });
        session.SetUrl(cpr::Url{ version_url });

        cpr::Response r = session.Get();
        if (r.status_code == 200) {
            LOG_INFO(std::format("[{}] Got response from: {} in {:.2f}s", __FUNCTION__, r.url.c_str(), r.elapsed));
            json j = json::parse(r.text);

            for (auto& [key, value] : j["game_ver"].items()) {
                // LOG_INFO(std::format("[{}] Game Version: {} Title Update: {}", __FUNCTION__, key.c_str(), value.get<std::string>().c_str()));
                game_version_map[key] = value.get<std::string>();
            }

            latest_tool_version = j["le_ver"]["silver"]["ver"].get<std::string>();
            latest_version_url = j["le_ver"]["silver"]["link"].get<std::string>();

            // LOG_INFO(std::format("Latest LE Ver: {} ({})", latest_tool_version.c_str(), latest_version_url.c_str()));

            if (j["compatibility"].contains(game_version) && j["compatibility"].at(game_version).is_array()) {
                compatible_range_low = j["compatibility"].at(game_version).at(0).get<std::string>();
                compatible_range_high = j["compatibility"].at(game_version).at(1).get<std::string>();

                // LOG_INFO(std::format("[{}] Game Version: {} Compatibility Range: {} - {}", __FUNCTION__, game_version.c_str(), compatible_range_low.c_str(), compatible_range_high.c_str()));
            }
        }
        else {
            LOG_ERROR(std::format(
                "[{}] Can't get response from: {}. status code: {}. Error: {}",
                __FUNCTION__, r.url.c_str(), r.status_code, r.error.message.c_str()
            ));

            if (game_version_compatibility.contains(game_version)) {
                // Use if offline
                compatible_range_low = game_version_compatibility[game_version][0];
                compatible_range_high = game_version_compatibility[game_version][1];
            }
            LOG_INFO(std::format("[{}] Game Version: {} Offline Compatibility Range: {} - {}", __FUNCTION__, game_version.c_str(), compatible_range_low.c_str(), compatible_range_high.c_str()));
        }

        UpdateCompatibility(compatible_range_low, compatible_range_high);
        UpdateIsUsingLatestVersion();

        // Convert Game Version Number To Title Update Number
        if (game_version_map.contains(game_version)) {
            is_compatibility_known = true;
            game_version = game_version_map[game_version];
        }
        LOG_FUNC_END();
    }

    bool VersionManager::IsUsingLatestVersion() {
        return is_using_latest_le;
    }

    bool VersionManager::IsCompatibilityKnown() {
        return is_compatibility_known;
    }

    bool VersionManager::IsCompatible() {
        return is_compatible;
    }

    const char* VersionManager::GetLatestVersion() {
        return latest_tool_version.c_str();
    }

    const char* VersionManager::GetLatestVersionURL() {
        return latest_version_url.c_str();
    }

    const char* VersionManager::GetToolVersion() {
        return tool_version.c_str();
    }

    const char* VersionManager::GetGameVersion() {
        return game_version.c_str();
    }

    std::string VersionManager::GetLocalGameVersion() {
        LOG_INFO(std::format("[{}]", __FUNCTION__));
        std::string result = "0.0.0.0";

        auto game_loc = LE::FilesManager::GetInstance()->GetGameDirectory();
        if (game_loc.empty() || !fs::exists(game_loc)) {
            return result;
        }

        fs::path fpath = game_loc / "__Installer" / "installerdata.xml";
        if (!fs::exists(fpath)) {
            LOG_ERROR(std::format("[{}] Can't find {}", __FUNCTION__, ToUTF8String(fpath).c_str()));
            return result;
        }

        FILE* f = _wfopen(fpath.wstring().c_str(), L"rb");
        if (!f) {
            LOG_ERROR(std::format("[{}] Can't open {}", __FUNCTION__, ToUTF8String(fpath).c_str()));
            return result;
        }

        fseek(f, 0, SEEK_END);
        __int64 fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (fsize <= 0) {
            LOG_ERROR(std::format("[{}] File is empty(?) {}", __FUNCTION__, ToUTF8String(fpath).c_str()));
            return result;
        }

        char* fbuf = new char[fsize];
        fread(fbuf, fsize, 1, f);
        fclose(f);

        tinyxml2::XMLDocument xmlDoc;
        xmlDoc.Parse(fbuf, fsize);
        tinyxml2::XMLElement* DiPManifest = xmlDoc.FirstChildElement("DiPManifest");
        if (DiPManifest) {
            tinyxml2::XMLElement* buildMetaData = DiPManifest->FirstChildElement("buildMetaData");
            if (buildMetaData) {
                tinyxml2::XMLElement* gameVersion = buildMetaData->FirstChildElement("gameVersion");
                if (gameVersion) {
                    result = std::string(gameVersion->Attribute("version"));
                }
            }
        }

        delete[] fbuf;
        return result;
    }

    void VersionManager::UpdateIsUsingLatestVersion() {
        int int_toolver = GetVersionAsInt(tool_version);
        int int_latest_tool_ver = GetVersionAsInt(latest_tool_version);

        if (int_toolver < int_latest_tool_ver) {
            is_using_latest_le = false;
        }
    }

    void VersionManager::UpdateCompatibility(std::string first_compatible, std::string last_compatible) {
        if (first_compatible.empty() || last_compatible.empty()) {
            is_compatible = false;
            return;
        }

        int int_toolver = GetVersionAsInt(tool_version);
        int int_first_compatible = GetVersionAsInt(first_compatible);
        int int_last_compatible = GetVersionAsInt(last_compatible);

        if (int_toolver < int_first_compatible || int_toolver > int_last_compatible) {
            is_compatible = false;
        }
    }

    int VersionManager::GetVersionAsInt(std::string version) {
        version.erase(std::remove(version.begin(), version.end(), 'v'), version.end());
        version.erase(std::remove(version.begin(), version.end(), '.'), version.end());
        return std::stoi(version);
    }

    void VersionManager::ValidateDLL() {
        std::string fpath = "FCLiveEditor.DLL";

        if (!std::filesystem::exists(fpath)) {
            LOG_ERROR("Can't validate DLL. File not exist");
            return;
        }

        std::ifstream file(fpath, std::ifstream::binary);
        MD5_CTX md5Context;
        MD5_Init(&md5Context);
        char buf[1024 * 16];
        while (file.good()) {
            file.read(buf, sizeof(buf));
            MD5_Update(&md5Context, buf, file.gcount());
        }
        unsigned char result[MD5_DIGEST_LENGTH];
        MD5_Final(result, &md5Context);

        std::stringstream md5string;
        md5string << std::hex << std::uppercase << std::setfill('0');
        for (const auto& byte : result)
            md5string << std::setw(2) << (int)byte;

        LOG_INFO(std::format("DLL MD5: {}", md5string.str().c_str()));
    }

    VersionManager* VersionManager::GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
            pinstance_ = new VersionManager();

        return pinstance_;
    }
}

LE::VersionManager* LE::VersionManager::pinstance_{ nullptr };
std::mutex LE::VersionManager::mutex_;