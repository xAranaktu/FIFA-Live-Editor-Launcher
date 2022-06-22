#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <aclapi.h>
#include <sddl.h>

#include <logger.h>
#include <core.h>

namespace fs = std::filesystem;

// DLL Injector
class Injector
{
public:
    enum STATUS {
        STATUS_INVALID = -1,
        STATUS_IDLE = 0,
        STATUS_WAITING,
        STATUS_INJECTING,
        STATUS_DONE,
        STATUS_ERROR
    };

    std::vector<std::string> status_names = {
        "Idle",
        "Waiting",
        "Injecting",
        "Done",
        "Error"
    };

    Injector();
    ~Injector();

    void SetStatus(STATUS _status);
    STATUS GetStatus();
    std::string GetStatusName();

    int GetGamePID();

    void Inject();

private:
    std::mutex m_status;
    STATUS injection_status = STATUS_IDLE;

    bool SetAccessControl(const wchar_t* file, const wchar_t* access);
    bool HasBlacklistedModule(const int pid);
};

extern Injector g_Injector;