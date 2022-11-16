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

bool Injector::HasBlacklistedModule(const int pid) {
    bool status = false;
    PVOID snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    MODULEENTRY32 modEntry = { sizeof(MODULEENTRY32) };
    while (Module32Next(snapshot, &modEntry)) {
        for (std::string const name : g_Config.dlls) {
            if (strcmp(modEntry.szModule, name.c_str()) == 0) {
                logger.Write(LOG_ERROR, "Found %s module", name.c_str());
                status = true;
                break;
            }
        }
    }
    CloseHandle(snapshot);
    return status;
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
                break;
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

    auto game_process_id = 0;
    while (true)
    {
        game_process_id = GetGamePID(gamepid);

        if (game_process_id > 0) {
            break;
        }

        Sleep(100);
    }

    HANDLE process = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, 
        false, 
        game_process_id
    );
    if (!process || process == INVALID_HANDLE_VALUE)
    {
        logger.Write(LOG_ERROR, "[%s] Can't open game process. Error: %d. PID: %d", __FUNCTION__, GetLastError(), game_process_id);
        SetStatus(STATUS_ERROR);
        return;
    }

    //if (HasBlacklistedModule(gamepid)) {
    //    logger.Write(LOG_ERROR, "[%s] Terminating injection because of blacklisted module", __FUNCTION__);
    //    SetStatus(STATUS_ERROR);
    //    return;
    //}

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
            auto err_code = GetLastError();

            logger.Write(LOG_ERROR, "[%s] Failed to alloc %llu bytes. Error code: %d", __FUNCTION__, len, err_code);
            SetStatus(STATUS_ERROR);

            if (err_code == 5) {
                MessageBox(NULL, "Failed to allocate space.\nDid you disabled the EA Anticheat?", "Failed", MB_ICONERROR);
            }

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

    SetStatus(STATUS_DONE);
}

Injector g_Injector;
