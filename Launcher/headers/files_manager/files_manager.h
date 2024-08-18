#pragma once
#include <Windows.h>
#include <mutex>
#include <filesystem>
#include <format>
#include <Psapi.h>

#include <consts.h>
#include <logger/logger.h>
#include <config.h>

namespace fs = std::filesystem;

namespace LE {
    class FilesManager
    {
    public:
        FilesManager(FilesManager& other) = delete;
        void operator=(const FilesManager&) = delete;
        static FilesManager* GetInstance();

#pragma region EAAC
        void BackupAnticheat();
        void RestoreAnticheat();
        void InstallFakeAnticheat();
        void UnInstallFakeAnticheat();
#pragma endregion EAAC

        void CreateDirectories();

        fs::path GetRoot();
        fs::path GetGameDirectory();
        fs::path GetLEDataDirectory();
        fs::path GetLEModsDirectory();
        fs::path GetKeysDirectory();

        // Full file paths
        fs::path GetAnticheatLauncherPath();
        fs::path GetFakeAnticheatLauncherPath();
        fs::path GetLangPath();
        fs::path GetConfigPath();
        fs::path GetLocaleKeyPath();
        fs::path GetGameProcessFullPath();
        const char* GetImGuiIni();

        // Detect 3rd Party Mods
        void DetectFIFAModManager();
        void DetectAnadius();

        // Logger
        void SetupLogger();

        // Config
        void SetupConfig();

        bool SetCustomGameDir(fs::path _dir);
        bool SetLEDataDir(fs::path _dir);
        bool SetLEModsDir(fs::path _dir);

#pragma region UTF8Directories
        // Directories
        std::string GetGameDirectoryU8();
        std::string GetLEDataDirectoryU8();
        std::string GetLEModsDirectoryU8();
#pragma endregion UTF8Directories

    private:
        static FilesManager* pinstance_;
        static std::mutex mutex_;

        fs::path tool_root;
        fs::path game_root;
        fs::path data_root;
        fs::path mods_root;

        std::string ToUTF8String(const std::filesystem::path& value);

        fs::path GetGameInstallDirFromReg();
        fs::path GetCustomGameInstallDirFromReg();
        fs::path GetLEDataDirFromReg();
        fs::path GetLEModsDirFromReg();

        fs::path GetLERegPathKey(const char* value_name);
        bool SetLERegPathKey(const wchar_t* value_name, fs::path _dir);

        void SafeCreateDirectories(fs::path _dir);
    protected:
        FilesManager();
        ~FilesManager();
    };
}