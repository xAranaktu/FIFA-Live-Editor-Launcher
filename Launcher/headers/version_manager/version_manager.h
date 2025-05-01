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
        std::string latest_tool_version = "v25.3.6";
        std::string tool_version = "v25.3.6";
        std::string game_version = "1.0.0.0";

        std::map<std::string, std::string> game_version_map = {
            { "1.0.122.27667",      "TU14"},
            { "1.0.121.45135",      "TU13"},
            { "1.0.120.62184",      "TU12"},
            { "1.0.120.39585",      "TU11"},
            { "1.0.120.7387",       "TU10"},
            { "1.0.119.18738",      "TU9.1"},
            { "1.0.118.64604",      "TU9"},
            { "1.0.118.36628",      "TU8"},
            { "1.0.117.63822",      "TU7"},
            { "1.0.117.47204",      "TU6.1"},
            { "1.0.117.36387",      "TU6"},
            { "1.0.117.17964",      "TU5"},
            { "1.0.116.40849",      "TU4"},
            { "1.0.116.27276",      "TU3"},
            { "1.0.116.10209",      "TU2"},
            { "1.0.116.295",        "TU1"},
            { "1.0.115.53355",      "Vanilla"}
        };

        std::map<std::string, std::vector<std::string>> game_version_compatibility = {
            { "1.0.122.27667",      { "v25.3.5", "v25.3.6" }},
            { "1.0.121.45135",      { "v25.3.2", "v25.3.4" }},
            { "1.0.120.62184",      { "v25.3.2", "v25.3.4" }},
            { "1.0.120.39585",      { "v25.3.2", "v25.3.4" }},
            { "1.0.120.7387",       { "v25.3.1", "v25.3.1" }},
            { "1.0.119.18738",      { "v25.2.8", "v25.3.0" }},
            { "1.0.118.64604",      { "v25.2.8", "v25.3.0" }},
            { "1.0.118.36628",      { "v25.1.3", "v25.2.7" }},
            { "1.0.117.63822",      { "v25.1.3", "v25.2.7" }},
            { "1.0.117.47204",      { "v25.1.3", "v25.2.7" }},
            { "1.0.117.36387",      { "v25.1.3", "v25.2.7" }},
            { "1.0.117.17964",      { "v25.1.3", "v25.2.7" }},
            { "1.0.116.40849",      { "v25.1.3", "v25.2.7" }},
            { "1.0.116.27276",      { "v25.1.3", "v25.2.7" }},
            { "1.0.116.10209",      { "v25.1.3", "v25.2.7" }},
            { "1.0.116.295",        { "v25.1.0", "v25.2.2" }},
            { "1.0.115.53355",      { "v25.1.0", "v25.1.2" }}
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