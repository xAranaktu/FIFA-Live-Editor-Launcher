#include <injector.h>

Injector::Injector()
{
}

Injector::~Injector()
{
}

void Injector::SetDelay(int d) {
    delay = d;
}

void Injector::SetStatus(STATUS _status) {
    std::lock_guard<std::mutex> lk(m_status);

    injection_status = _status;
}
Injector::STATUS Injector::GetStatus() {
    std::lock_guard<std::mutex> lk(m_status);

    return injection_status;
}

void Injector::SetInterupt(bool _interupt) {
    std::lock_guard<std::mutex> lk(m_interupt);

    should_interupt = _interupt;
}
bool Injector::GetInterupt() {
    std::lock_guard<std::mutex> lk(m_interupt);

    return should_interupt;
}

bool Injector::CanShutdown() {
    if (g_Config.close_after_injection) 
        return GetStatus() == STATUS::STATUS_DONE;

    return false;
}

std::string Injector::GetStatusName() {
    return status_names.at(GetStatus());
}

std::string Injector::GetStatusDesc() {
    return status_desc.at(GetStatus());
}

// https://www.unknowncheats.me/forum/general-programming-and-reversing/177183-basic-intermediate-techniques-uwp-app-modding.html
bool Injector::SetAccessControl(const wchar_t* file, const wchar_t* access)
{
    PSECURITY_DESCRIPTOR sec = nullptr;
    PACL currentAcl = nullptr;
    PSID sid = nullptr;
    PACL newAcl = nullptr;
    bool status = false;
    goto init;
end:
    if (newAcl) LocalFree(newAcl);
    if (sid) LocalFree(sid);
    if (sec) LocalFree(sec);
    return status;
init:
    if (GetNamedSecurityInfoW(file, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, &currentAcl, nullptr, &sec) != ERROR_SUCCESS) goto end;
    if (!ConvertStringSidToSidW(access, &sid)) goto end;
    EXPLICIT_ACCESSW desc = { 0 };
    desc.grfAccessPermissions = GENERIC_READ | GENERIC_EXECUTE | GENERIC_WRITE;
    desc.grfAccessMode = SET_ACCESS;
    desc.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    desc.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    desc.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    desc.Trustee.ptstrName = reinterpret_cast<wchar_t*>(sid);
    if (SetEntriesInAclW(1, &desc, currentAcl, &newAcl) != ERROR_SUCCESS) goto end;
    if (SetNamedSecurityInfoW(const_cast<wchar_t*>(file), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, newAcl, nullptr) != ERROR_SUCCESS) goto end;
    status = true;
    goto end;
}

int Injector::GetGamePID(int invalid) {
    PVOID snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(process);

    DWORD pid = 0;
    while (Process32Next(snapshot, &process)) {
        for (std::string const name : g_Config.proc_names) {
            if (strcmp(process.szExeFile, name.c_str()) == 0) {
                pid = process.th32ProcessID;
                if (pid == invalid) {
                    pid = 0;
                    continue;
                }

                logger.Write(LOG_INFO, "Found %s (PID: %d)", process.szExeFile, pid);
            }
        }
    }

    CloseHandle(snapshot);
    return pid;
}

bool Injector::AnticheatDetected() {
    PVOID snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(process);

    DWORD pid = 0;
    while (Process32Next(snapshot, &process)) {
        for (std::string const name : g_Config.anticheat_proc_names) {
            if (strcmp(process.szExeFile, name.c_str()) == 0) {
                pid = process.th32ProcessID;
                logger.Write(LOG_INFO, "Found Anticheat %s (PID: %d)", name.c_str(), pid);
                break;
            }
        }
    }

    CloseHandle(snapshot);
    return pid;
}

void Injector::Inject() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    std::vector<fs::path> fulldll_dirs;
    for (auto dll : g_Config.dlls) {
        fs::path fulldll_dir = g_Core.ctx.GetFolder() + "\\" + dll;
        logger.Write(LOG_INFO, "[%s] DLL dir: %s", __FUNCTION__, fulldll_dir.string().c_str());
        if (!fs::exists(fulldll_dir)) {
            logger.Write(LOG_ERROR, "[%s] Can't find DLL at %s", __FUNCTION__, fulldll_dir.string().c_str());
            SetStatus(STATUS_ERROR);
            return;
        }
        fulldll_dirs.push_back(fulldll_dir);
    }

    logger.Write(LOG_INFO, "[%s] Trying to inject into one of the following procnames:", __FUNCTION__);
    for (std::string const name : g_Config.proc_names) {
        logger.Write(LOG_INFO, "%s", name.c_str());
    }

    int gamepid = 0;
    SetStatus(STATUS_WAITING_FOR_GAME);
    logger.Write(LOG_INFO, "[%s] STATUS_WAITING_FOR_GAME", __FUNCTION__);
    while (true)
    {
        if (GetInterupt()) {
            logger.Write(LOG_INFO, "[%s] Interupting injection...", __FUNCTION__);
            SetStatus(STATUS_IDLE);
            SetInterupt(false);
            return;
        }

        gamepid = GetGamePID();
        if (gamepid > 0) break;

        Sleep(100);
    }

    SetStatus(STATUS_INJECTING);
    logger.Write(LOG_INFO, "[%s] STATUS_INJECTING, delay %d ms", __FUNCTION__, delay);
    Sleep(delay);

    HANDLE process = NULL;
    int game_process_id = 0;
    while (true)
    {
        // OpenProcess1 when auto inject to already started process
        {
            process = OpenProcess(
                PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
                false,
                gamepid
            );
            if (process != NULL) {
                logger.Write(LOG_INFO, "[%s] OpenProcess1 %d", __FUNCTION__, gamepid);
                break;
            }
            else {
                DWORD err = GetLastError();
                logger.Write(LOG_WARN, "[%s] OpenProcess1 failed: %s (%d) PID: %d", __FUNCTION__, std::system_category().message(err).c_str(), err, gamepid);
            }
        }

        // OpenProcess2 when using "run game" button
        {
            game_process_id = GetGamePID(gamepid);
            if (game_process_id > 0) {
                process = OpenProcess(
                    PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
                    false,
                    game_process_id
                );
                if (process != NULL) {
                    logger.Write(LOG_INFO, "[%s] OpenProcess2 %d", __FUNCTION__, game_process_id);
                    break;
                }
                else {
                    DWORD err = GetLastError();
                    logger.Write(LOG_WARN, "[%s] OpenProcess2 failed: %s (%d) PID: %d", __FUNCTION__, std::system_category().message(err).c_str(), err, game_process_id);
                }
            }
        }

        Sleep(100);
    }

    for (auto dll : fulldll_dirs) {
        auto dll_module = dll.wstring();

        if (!SetAccessControl(dll_module.c_str(), L"S-1-15-2-1")) {
            logger.Write(LOG_ERROR, "[%s] SetAccessControl Failed", __FUNCTION__);
            SetStatus(STATUS_ERROR);
            return;
        }

        auto len = dll_module.capacity() * sizeof(wchar_t);
        LPVOID alloc = VirtualAllocEx(process, 0, len, MEM_COMMIT, PAGE_READWRITE);
        if (!alloc) {
            DWORD err = GetLastError();
            std::string err_msg = std::system_category().message(err);

            logger.Write(LOG_ERROR, "[%s] Failed to alloc %llu bytes. Error: %s (%d)", __FUNCTION__, len, err_msg.c_str(), err);
            SetStatus(STATUS_ERROR);

            MessageBox(NULL, "Failed to allocate space.\nDid you disabled the EA Anticheat?", "Failed", MB_ICONERROR);

            return;
        }
        if (!WriteProcessMemory(process, alloc, dll_module.data(), len, 0)) {
            logger.Write(LOG_ERROR, "[%s] WriteProcessMemory Failed", __FUNCTION__);
            SetStatus(STATUS_ERROR);
            return;
        }
        auto thread = CreateRemoteThread(process, 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), alloc, 0, 0);
        if (!thread) {
            logger.Write(LOG_ERROR, "[%s] CreateRemoteThread Failed", __FUNCTION__);
            SetStatus(STATUS_ERROR);
            return;
        }
        WaitForSingleObject(thread, INFINITE);
        VirtualFreeEx(process, alloc, len, MEM_RESERVE);
    }
    CloseHandle(process);

    SetStatus(STATUS_DONE);
}

Injector g_Injector;
