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
        std::string latest_tool_version = "v26.1.0";
        std::string tool_version = "v26.1.0";
        std::string game_version = "1.0.0.0";

        std::map<std::string, std::string> game_version_map = {
            { "1.0.126.61236",      "CB TU1"},
            { "1.0.126.39636",      "CB Vanilla"}
        };

        std::map<std::string, std::vector<std::string>> game_version_compatibility = {
            { "1.0.126.61236",      { "v26.1.0", "v26.1.0" }},
            { "1.0.126.39636",      { "v26.1.0", "v26.1.0" }}
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