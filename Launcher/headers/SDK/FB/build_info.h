#pragma once

#include <Windows.h>
#include <filesystem>

// https://www.unknowncheats.me/forum/frostbite/162503-buildinfo.html
class BuildInfo
{
public:
    virtual const char* getBranchName() const = 0;
    virtual const char* getHostName() const = 0;
    virtual const char* getLicenseeId() const = 0;
    virtual const char* getStudioName() const = 0;
    virtual int getChangelist() const = 0;
    virtual int getSourceChangelist() const = 0;
    virtual int getFrostbiteChangelist() const = 0;
    virtual const char* getFrostbiteRelease() const = 0;
    virtual const char* getFrostbiteReleaseAlt() const = 0;
    virtual bool getIsAutoBuild() const = 0;
    virtual const char* getUsername() const = 0;
    virtual const char* getBuildTime() const = 0;
    virtual const char* getBuildDate() const = 0;
    virtual const char* getBuildIsoDate() const = 0;

    virtual const char* getNullPtr() const = 0;
    virtual const char* getNullPtr2() const = 0;
    virtual const char* getNullPtr3() const = 0;
    virtual int getInt4() const = 0;

    static BuildInfo* GetInstance(std::filesystem::path dll_path)
    {
        HMODULE hBuildInfoDLL = LoadLibrary(dll_path.string().c_str());
        if (!hBuildInfoDLL) {
            return 0;
        }

        typedef BuildInfo* (__cdecl* getBuildInfo_t)(void);
        getBuildInfo_t getBuildInfo = (getBuildInfo_t)GetProcAddress(hBuildInfoDLL, "getBuildInfo");

        return getBuildInfo();
    }
};
