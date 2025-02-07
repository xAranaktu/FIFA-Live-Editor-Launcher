#include <version_manager/version_manager.h>

namespace LE {
    VersionManager::VersionManager() {}
    VersionManager::~VersionManager() {}

    void VersionManager::Init() {
        LOG_INFO(std::format("[{}]", __FUNCTION__));

        game_version = GetLocalGameVersion();
        LOG_INFO(std::format("Game Version: {}", game_version.c_str()));

        std::thread t1(&LE::VersionManager::CheckUpdates, this);
        t1.detach();
    }

    void VersionManager::CheckUpdates() {
        LOG_FUNC_START();
        std::string base_url = "https://raw.githubusercontent.com/xAranaktu/FC-25-Live-Editor";
        std::string game_ver_json = std::format("{}/refs/heads/main/GAME_VER.json", base_url);
        std::string le_compatibility_json = std::format("{}/refs/heads/main/LE_COMPATIBILITY.json", base_url);
        std::string le_ver_json = std::format("{}/refs/heads/main/LE_VER.json", base_url);

        cpr::Session session;
        session.SetHeader(cpr::Header{ { "User-Agent", std::format("FC Live Editor {}", tool_version) } });

        // Check game version
        session.SetUrl(cpr::Url{ game_ver_json });
        cpr::Response r = session.Get();

        if (r.status_code == 200) {
            LOG_INFO(std::format("[{}] Got response from: {} in {}", __FUNCTION__, r.url.c_str(), r.elapsed));
            json j = json::parse(r.text);

            for (auto& [key, value] : j.items()) {
                game_version_map[key] = value.get<std::string>();
            }
        }
        else {
            LOG_ERROR(std::format(
                "[{}] Can't get response from: {}. status code: {}. Error: {}",
                __FUNCTION__, r.url.c_str(), r.status_code, r.error.message.c_str()
            ));
        }

        // Check LE Version
        session.SetUrl(cpr::Url{ le_ver_json });
        r = session.Get();
        if (r.status_code == 200) {
            LOG_INFO(std::format("[{}] Got response from: {} in {}", __FUNCTION__, r.url.c_str(), r.elapsed));
            json j = json::parse(r.text);

            latest_tool_version = j["silver"]["ver"].get<std::string>();
            latest_version_url = j["silver"]["link"].get<std::string>();
        }

        // Check LE Compatibility
        std::string compatible_range_low;
        std::string compatible_range_high;
        session.SetUrl(cpr::Url{ le_compatibility_json });
        r = session.Get();

        if (r.status_code == 200) {
            LOG_INFO(std::format("[{}] Got response from: {} in {}", __FUNCTION__, r.url.c_str(), r.elapsed));
            json j = json::parse(r.text);

            if (j.contains(game_version) && j.at(game_version).is_array()) {
                compatible_range_low = j.at(game_version).at(0).get<std::string>();
                compatible_range_high = j.at(game_version).at(1).get<std::string>();

                LOG_INFO(std::format("[{}] Game Version: {} Compatibility Range: {} - {}", __FUNCTION__, game_version.c_str(), compatible_range_low.c_str(), compatible_range_high.c_str()));
            }
        }
        else {
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