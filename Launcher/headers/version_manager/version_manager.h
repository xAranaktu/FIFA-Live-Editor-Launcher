#pragma once
#include <mutex>
#include <string>
#include <map>
#include <vector>
#include <filesystem>
#include <openssl/md5.h>

#include <files_manager/files_manager.h>

#include "tinyxml2.h"
#include <cpr/cpr.h>

namespace LE {
    class VersionManager {
    public:
        VersionManager(VersionManager& other) = delete;
        void operator=(const VersionManager&) = delete;
        static VersionManager* GetInstance();

        void Init();

        void CheckUpdates();

        bool IsUsingLatestVersion();
        bool IsCompatibilityKnown();
        bool IsCompatible();

        const char* GetLatestVersion();
        const char* GetLatestVersionURL();
        const char* GetToolVersion();
        const char* GetGameVersion();

    private:
        static VersionManager* pinstance_;
        static std::mutex mutex_;

        bool is_compatibility_known = false;
        bool is_compatible = true;
        bool is_using_latest_le = true;

        std::string latest_version_url = "";
        std::string latest_tool_version = "v26.2.6";
        std::string tool_version = "v26.2.6";
        std::string game_version = "1.0.0.0";

        std::map<std::string, std::string> game_version_map = {
            { "1.0.131.24706",      "v1.4.2"},
            { "1.0.130.35129",      "v1.4.1"},
            { "1.0.130.16994",      "v1.4.0"},
            { "1.0.129.30822",      "v1.3.0"},
            { "1.0.129.25108",      "v1.2.1"},
            { "1.0.129.4059",       "v1.2.0"},
            { "1.0.129.1902",       "v1.1.3"},
            { "1.0.128.63165",      "v1.1.2"},
            { "1.0.128.60171",      "v1.1.1"},
            { "1.0.128.37607",      "v1.1.0"},
            { "1.0.128.29120",      "v1.0.4"},
            { "1.0.128.17361",      "v1.0.3"},
            { "1.0.128.4697",       "v1.0.2"},
            { "1.0.127.59053",      "Vanilla"},
        };

        std::map<std::string, std::vector<std::string>> game_version_compatibility = {
            { "1.0.131.24706",      { "v26.2.3", "v26.2.6" }},
            { "1.0.130.35129",      { "v26.2.3", "v26.2.6" }},
            { "1.0.130.16994",      { "v26.2.3", "v26.2.6" }},
            { "1.0.129.30822",      { "v26.1.9", "v26.2.2" }},
            { "1.0.129.25108",      { "v26.1.9", "v26.2.2" }},
            { "1.0.129.4059",       { "v26.1.9", "v26.2.2" }},
            { "1.0.129.1902",       { "v26.1.6", "v26.1.8" }},
            { "1.0.128.63165",      { "v26.1.6", "v26.1.8" }},
            { "1.0.128.60171",      { "v26.1.6", "v26.1.8" }},
            { "1.0.128.37607",      { "v26.1.6", "v26.1.8" }},
            { "1.0.128.29120",      { "v26.1.0", "v26.1.5" }},
            { "1.0.128.17361",      { "v26.1.0", "v26.1.5" }},
            { "1.0.128.4697",       { "v26.1.0", "v26.1.5" }}
        };

        void from_json(const json& j);

        void DownloadVersionFile();

        std::string GetLocalGameVersion();
        void UpdateIsUsingLatestVersion();
        void UpdateCompatibility(std::string first_compatible, std::string last_compatible);

        int GetVersionAsInt(std::string version);

        void ValidateGameProc();
        void ValidateDLL();

    protected:
        VersionManager();
        ~VersionManager();
    };
}