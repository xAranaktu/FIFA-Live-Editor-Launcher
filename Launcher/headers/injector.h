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
        STATUS_WAITING_FOR_GAME,
        STATUS_WAITING_FOR_WINDOW,
        STATUS_INJECTING,
        STATUS_DONE,
        STATUS_ERROR
    };

    std::vector<std::string> status_names = {
        "Idle",
        "Waiting for Game",
        "Waiting for Window",
        "Injecting",
        "Done",
        "Error"
    };

    std::vector<std::string> status_desc = {
        "STATUS IDLE\nRun the game and hit Inject button/enable auto injection",
        "STATUS WAITING\nWaiting for the game process to appear. You need to run the game now.",
        "STATUS WAITING\nWaiting for the game window.",
        "STATUS INJECTING\nInjecting",
        "STATUS DONE\nDone",
        "STATUS ERROR\nInjection failed for some reasons. You should be able to find more details in the log file"
    };

    Injector();
    ~Injector();

    void SetDelay(int d);
    void SetStatus(STATUS _status);
    STATUS GetStatus();
    std::string GetStatusName();
    std::string GetStatusDesc();

    void SetInterupt(bool _interupt);
    bool GetInterupt();

    bool CanShutdown();

    int GetGamePID(int invalid = 0);
    bool AnticheatDetected();

    void Inject();

private:
    std::mutex m_status;
    std::mutex m_interupt;
    STATUS injection_status = STATUS_IDLE;

    int delay = 0;
    bool should_interupt = false;

    bool SetAccessControl(const wchar_t* file, const wchar_t* access);
};

extern Injector g_Injector;