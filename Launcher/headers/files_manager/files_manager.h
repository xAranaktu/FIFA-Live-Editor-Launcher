#pragma once
#include <Windows.h>
#include <mutex>
#include <filesystem>
#include <format>
#include <Psapi.h>

#include <consts.h>
#include <logger/logger.h>

namespace fs = std::filesystem;

namespace LE {
    class FilesManager
    {
    public:
        FilesManager(FilesManager& other) = delete;
        void operator=(const FilesManager&) = delete;
        static FilesManager* GetInstance();

        void BackupAnticheat();
        void RestoreAnticheat();
        void InstallFakeAnticheat();
        void UnInstallFakeAnticheat();

        void CreateDirectories();

        fs::path GetRoot();

        fs::path GetGamePath();
        fs::path GetLEDataPath();
        fs::path GetLEModsPath();

        fs::path GetAnticheatLauncherPath();
        fs::path GetFakeAnticheatLauncherPath();

        // Detect 3rd Party Mods
        void DetectFIFAModManager();
        void DetectAnadius();

        // Logger
        void SetupLogger();

    private:
        static FilesManager* pinstance_;
        static std::mutex mutex_;

        fs::path tool_root;

        std::string ToUTF8String(const std::filesystem::path& value);

        fs::path GetGameInstallDirFromReg();
        fs::path GetLEDataDirFromReg();
        fs::path GetLEModsDirFromReg();

        bool SetLEDataDir(fs::path _dir);
        bool SetLEModsDir(fs::path _dir);

        fs::path GetLERegPathKey(const char* value_name);
        bool SetLERegPathKey(const wchar_t* value_name, fs::path _dir);

        void SafeCreateDirectories(fs::path _dir);
    protected:
        FilesManager();
        ~FilesManager();
    };
}