#pragma once
#include <psapi.h>
#include <string>

//https://github.com/E66666666/GTAVManualTransmission/blob/b2854a1896456c80826e076a2e99131acd2281b6/Gears/Util/Paths.cpp
namespace core
{
    class Context
    {
    public:

        struct ModuleContext
        {
            std::string m_Name = "";
            uintptr_t m_Base = NULL;
            uintptr_t m_Size = NULL;
        } m_ModuleContext;

        HMODULE             m_ModuleHandle = NULL;
        std::string         sfullModPath = "";

        std::string GetFolder()
        {
            return sfullModPath.substr(0, sfullModPath.find_last_of("\\"));
        };

        std::string GetName()
        {
            size_t lastIndex = sfullModPath.find_last_of("\\") + 1;
            return sfullModPath.substr(lastIndex, sfullModPath.length() - lastIndex);
        };

        void Update(HMODULE hModule)
        {
            m_ModuleHandle = hModule;
            MODULEINFO modinfo;
            HANDLE hProcess = GetCurrentProcess();

            if (GetModuleInformation(hProcess, hModule, &modinfo, sizeof(MODULEINFO)))
            {
                m_ModuleContext.m_Base = reinterpret_cast<uintptr_t>(modinfo.lpBaseOfDll);
                m_ModuleContext.m_Size = static_cast<uintptr_t>(modinfo.SizeOfImage);
            }
            char fullModPath[MAX_PATH];
            if (GetModuleFileNameA(hModule, fullModPath, MAX_PATH))
            {
                sfullModPath = fullModPath;
                m_ModuleContext.m_Name = GetName();
            }
        }
    };
}