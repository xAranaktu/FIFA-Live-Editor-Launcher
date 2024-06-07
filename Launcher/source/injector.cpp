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
    if (g_Config.launch_values.close_after_injection) 
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
    bool result = false;
    PACL currentAcl = nullptr;
    PACL newAcl = nullptr;
    PSECURITY_DESCRIPTOR sec = nullptr;
    PSID sid = nullptr;

    if (GetNamedSecurityInfoW(
        file, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, &currentAcl, nullptr, &sec
    ) != ERROR_SUCCESS)
        return result;

    if (!ConvertStringSidToSidW(access, &sid))
        return result;


    EXPLICIT_ACCESSW desc = { 0 };
    desc.grfAccessPermissions = GENERIC_READ | GENERIC_EXECUTE | GENERIC_WRITE;
    desc.grfAccessMode = SET_ACCESS;
    desc.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    desc.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    desc.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    desc.Trustee.ptstrName = reinterpret_cast<wchar_t*>(sid);
    if (SetEntriesInAclW(1, &desc, currentAcl, &newAcl) == ERROR_SUCCESS) {
        SetNamedSecurityInfoW(const_cast<wchar_t*>(file), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, newAcl, nullptr);
        result = true;
    }

    if (newAcl) LocalFree(newAcl);
    if (sid)    LocalFree(sid);
    if (sec)    LocalFree(sec);

    return result;
}

std::vector<int> Injector::GetGamePIDs() {
    std::vector<int> result;

    /*
    DWORD pid = 0;
    std::wstring wsTmp1(g_Config.launch_values.game_proc_name.begin(), g_Config.launch_values.game_proc_name.end());
    std::wstring wsTmp2(g_Config.launch_values.game_proc_name_trial.begin(), g_Config.launch_values.game_proc_name_trial.end());

    const wchar_t* procname1 = wsTmp1.c_str();
    const wchar_t* procname2 = wsTmp2.c_str();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W process = { sizeof(process) };
    for (BOOL next = Process32FirstW(snapshot, &process); next; next = Process32NextW(snapshot, &process))
    {
        if (
            wcscmp(process.szExeFile, procname1) == 0 || 
            wcscmp(process.szExeFile, procname2) == 0
        ) {
            pid = process.th32ProcessID;
            result.push_back(pid);
            break;
        }
    }

    if (snapshot != NULL && snapshot != INVALID_HANDLE_VALUE) CloseHandle(snapshot);
    */

    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    DWORD aProcesses[2048], cbNeeded, cbNeededMod, cProcesses;
    HMODULE hMod;
    unsigned int i;

    if (!K32EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return result;
    }
    cProcesses = cbNeeded / sizeof(DWORD);
    for (i = 0; i < cProcesses; i++)
    {
        DWORD processID = aProcesses[i];
        if (processID == 0) continue;
        HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE, processID
        );
        if (hProcess == NULL) continue;

        if (K32EnumProcessModulesEx(hProcess, &hMod, sizeof(hMod), &cbNeededMod, LIST_MODULES_64BIT))
        {
            K32GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
        }

        if (
            strcmp(szProcessName, g_Config.launch_values.game_proc_name.c_str()) == 0 ||
            strcmp(szProcessName, g_Config.launch_values.game_proc_name_trial.c_str()) == 0
        ) {
            result.push_back(processID);
        }

        CloseHandle(hProcess);
    }

    return result;
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
        LPVOID alloc = VirtualAllocEx(process, 0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
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
        VirtualFreeEx(process, alloc, len, MEM_RELEASE);
    }
    CloseHandle(process);
    return true;
}

void Injector::Inject() {
    LOG_INFO(std::format("[{}]", __FUNCTION__));

    std::stringstream ssError;

    fulldll_dirs.clear();
    for (std::string dll : g_Config.launch_values.dlls) {
        fs::path fulldll_dir = g_Core.ctx.GetFolder() / dll;
        LOG_INFO(std::format("[{}] DLL dir: {}", __FUNCTION__, ToUTF8String(fulldll_dir).c_str()));
        if (!fs::exists(fulldll_dir)) {
            LOG_ERROR(std::format("[{}] Can't find DLL at {}", __FUNCTION__, ToUTF8String(fulldll_dir).c_str()));
            SetStatus(STATUS_ERROR);
            return;
        }
        fulldll_dirs.push_back(fulldll_dir);
    }

    LOG_INFO(std::format("[{}] Trying to inject into {} or {}", 
        __FUNCTION__,
        g_Config.launch_values.game_proc_name.c_str(),
        g_Config.launch_values.game_proc_name_trial.c_str()
    ));

    SetStatus(STATUS_WAITING_FOR_GAME);
    LOG_INFO("STATUS_WAITING_FOR_GAME");
    while (GetGamePIDs().empty())
    {
        if (Interupt()) return;

        Sleep(100);
    }

    SetStatus(STATUS_INJECTING);
    LOG_INFO(std::format("STATUS_INJECTING, delay {} ms", delay));
    Sleep(delay);

    bool success = true;
    HWND hWindow = NULL;
    // Max Wait ~60 seconds
    static int max_attempts = 1200;
    int attempts = 0;
    while (hWindow == NULL)
    {
        if (Interupt()) return;

        std::vector<int> proc_ids = GetGamePIDs();

        if (proc_ids.empty())   attempts++;

        if (attempts >= max_attempts) {
            LOG_ERROR("No game processes");
            ssError << "After several tries Live Editor wasn't able to find stable game process.\n";
            ssError << "Make sure that:\n";

            ssError << "1. You have extracted the Live Editor archive\n";
            ssError << "2. You are using latest Live Editor version\n";
            ssError << "3. Live Editor isn't blocked by your antivirus\n";
            ssError << "4. Your windows user have full admin rights\n";
            ssError << "5. You have cleared EA App Cache\n\n";

            ssError << "If you met all above conditions try to run the game without FIFAModManager mods\n";
            ssError << "Also, you can try to run Live Editor launcher and instead of pressing Run Game button try to start your game manually from EA App/Steam/Epic\n\n\n";

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

        hWindow = FindWindow("FC 24", 0);
        Sleep(30);
    }
    if (hWindow) {
        LOG_INFO("Game Window Found");
    }

    if (success) {
        SetStatus(STATUS_DONE);
    }
    else {
        LOG_ERROR("Injection failed...");

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

        LOG_ERROR(ssError.str());
        MessageBox(NULL, ssError.str().c_str(), "Failed", MB_ICONERROR);
    }
}

bool Injector::Interupt() {
    if (GetInterupt()) {
        LOG_INFO("Interupting injection..");
        SetStatus(STATUS_IDLE);
        SetInterupt(false);
        return true;
    }

    return false;
}

Injector g_Injector;
