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

std::vector<int> Injector::GetGamePIDs() {
    std::vector<int> result;

    PVOID snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(process);

    while (Process32Next(snapshot, &process)) {
        for (std::string const name : g_Config.proc_names) {
            if (strcmp(process.szExeFile, name.c_str()) == 0) {
                result.push_back(process.th32ProcessID);
                // logger.Write(LOG_INFO, "Found %s (PID: %d)", process.szExeFile, pid);
            }
        }
    }

    CloseHandle(snapshot);
    return result;
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

bool Injector::DoInjectDLL(int pid) {
    char buf_err[2048];
    memset(buf_err, 0, sizeof(buf_err));

    HANDLE process = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
        false,
        pid
    );

    if (!process) {
        DWORD err = GetLastError();
        sprintf_s(buf_err, sizeof(buf_err), "[%s] OpenProcess failed: %s (%d)",
            __FUNCTION__,
            std::system_category().message(err).c_str(),
            err
        );

        m_game_ids_errors[pid] = std::string(buf_err);

        return false;
    }

    for (auto dll : fulldll_dirs) {
        auto dll_module = dll.wstring();

        if (!SetAccessControl(dll_module.c_str(), L"S-1-15-2-1")) {
            sprintf_s(buf_err, sizeof(buf_err), "[%s] SetAccessControl Failed", __FUNCTION__);
            m_game_ids_errors[pid] = std::string(buf_err);

            CloseHandle(process);
            return false;
        }

        auto len = dll_module.capacity() * sizeof(wchar_t);
        LPVOID alloc = VirtualAllocEx(process, 0, len, MEM_COMMIT, PAGE_READWRITE);
        if (!alloc) {
            DWORD err = GetLastError();

            sprintf_s(buf_err, sizeof(buf_err), "[%s] Failed to alloc %llu bytes. Error: %s (%d)",
                __FUNCTION__,
                len,
                std::system_category().message(err).c_str(),
                err
            );
            m_game_ids_errors[pid] = std::string(buf_err);

            CloseHandle(process);
            return false;
        }
        if (!WriteProcessMemory(process, alloc, dll_module.data(), len, 0)) {
            sprintf_s(buf_err, sizeof(buf_err), "[%s] WriteProcessMemory Failed", __FUNCTION__);
            m_game_ids_errors[pid] = std::string(buf_err);

            CloseHandle(process);
            return false;
        }
        auto thread = CreateRemoteThread(process, 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), alloc, 0, 0);
        if (!thread) {
            sprintf_s(buf_err, sizeof(buf_err), "[%s] CreateRemoteThread Failed", __FUNCTION__);
            m_game_ids_errors[pid] = std::string(buf_err);

            CloseHandle(process);
            return false;
        }
        WaitForSingleObject(thread, INFINITE);
        VirtualFreeEx(process, alloc, len, MEM_RESERVE);
    }
    CloseHandle(process);
    return true;
}

void Injector::Inject() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

    std::stringstream ssError;

    fulldll_dirs.clear();
    for (std::string dll : g_Config.dlls) {
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

    SetStatus(STATUS_WAITING_FOR_GAME);
    logger.Write(LOG_INFO, "[%s] STATUS_WAITING_FOR_GAME", __FUNCTION__);
    while (GetGamePIDs().empty())
    {
        if (Interupt()) return;

        Sleep(10);
    }

    SetStatus(STATUS_INJECTING);
    logger.Write(LOG_INFO, "[%s] STATUS_INJECTING, delay %d ms", __FUNCTION__, delay);
    Sleep(delay);

    bool success = true;
    HWND hWindow = NULL;
    // Max Wait ~60 seconds
    int max_attempts = 1200;
    while (hWindow == NULL)
    {
        if (Interupt()) return;

        std::vector<int> proc_ids = GetGamePIDs();

        if (proc_ids.empty())   max_attempts--;

        if (max_attempts <= 0) {
            logger.Write(LOG_WARN, "[%s] No game processes", __FUNCTION__);
            ssError << "No game found after " << max_attempts << " attempts\n";
            success = false;
            break;
        }

        for (int pid : proc_ids) {
            if (m_game_ids.count(pid) == 0) {
                // Add Process ID
                m_game_ids[pid] = false;
            }
            else {
                if (!m_game_ids[pid]) {
                    m_game_ids[pid] = DoInjectDLL(pid);
                }
            }
        }

        hWindow = FindWindow("FIFA 23", 0);
        Sleep(50);
    }
    if (hWindow) {
        logger.Write(LOG_INFO, "[%s] Game Window Found", __FUNCTION__);
    }

    if (success) {
        SetStatus(STATUS_DONE);
    }
    else {
        logger.Write(LOG_ERROR, "[%s] Injection failed...", __FUNCTION__);

        SetStatus(STATUS_ERROR);

        for (const auto& [pid, _status] : m_game_ids) {
            if (_status) {
                ssError << "PID: " << pid << " STATUS: " << "OK\n";
            }
            else {
                if (m_game_ids_errors.count(pid) == 1) {
                    ssError << "PID: " << pid << " ERROR: " << m_game_ids_errors.at(pid) <<"\n";
                }
                else {
                    ssError << "PID: " << pid << " ERROR: " << "UNKNOWN\n";
                }
            }
        }

        logger.Write(LOG_ERROR, "[%s] %s", __FUNCTION__, ssError.str().c_str());
        MessageBox(NULL, ssError.str().c_str(), "Failed", MB_ICONERROR);
    }
}

bool Injector::Interupt() {
    if (GetInterupt()) {
        logger.Write(LOG_INFO, "[%s] Interupting injection...", __FUNCTION__);
        SetStatus(STATUS_IDLE);
        SetInterupt(false);
        return true;
    }

    return false;
}

Injector g_Injector;
