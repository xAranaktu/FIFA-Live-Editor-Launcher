#include <injector.h>

Injector::Injector()
{
}

Injector::~Injector()
{
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
        for (std::string const name : blacklist) {
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

int Injector::GetGamePID() {
    PVOID snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(process);

    DWORD pid = 0;
    while (Process32Next(snapshot, &process)) {
        for (std::string const name : procnames) {
            if (strcmp(process.szExeFile, name.c_str()) == 0) {
                pid = process.th32ProcessID;
                logger.Write(LOG_INFO, "Found %s (PID: %d)", name.c_str(), pid);
                break;
            }
        }
    }

    CloseHandle(snapshot);
    return pid;
}

void Injector::LoadBlackList() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);
    blacklist.clear();

    // TODO: Load from file?
    blacklist.push_back(dll.filename().string());

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

void Injector::LoadProcNames() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);
    procnames.clear();

    // TODO: Load from file?
    procnames.push_back("FIFA22.exe");
    procnames.push_back("FIFA22_Trial.exe");

    logger.Write(LOG_INFO, "[%s] Done", __FUNCTION__);
}

void Injector::Inject() {
    logger.Write(LOG_INFO, "[%s]", __FUNCTION__);
    LoadProcNames();
    LoadBlackList();

    logger.Write(LOG_INFO, "[%s] Trying to inject into one of the following procnames:", __FUNCTION__);
    for (std::string const name : procnames) {
        logger.Write(LOG_INFO, "%s", name.c_str());
    }

    int gamepid = 0;
    while (true)
    {
        gamepid = GetGamePID();
        if (gamepid > 0) break;

        Sleep(100);
    }

    const HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, gamepid);
    if (process == INVALID_HANDLE_VALUE)
    {
        logger.Write(LOG_ERROR, "[%s] Can't open game process.", __FUNCTION__);
        return;
    }

    if (HasBlacklistedModule(gamepid)) {
        logger.Write(LOG_ERROR, "[%s] Terminating injection because of blacklisted module", __FUNCTION__);
        return;
    }

    auto dll_module = dll.wstring();

    if (!SetAccessControl(dll_module.c_str(), L"S-1-15-2-1")) {
        logger.Write(LOG_ERROR, "[%s] SetAccessControl Failed", __FUNCTION__);
        return;
    }

    auto len = dll_module.capacity() * sizeof(wchar_t);
    LPVOID alloc = VirtualAllocEx(process, 0, len, MEM_COMMIT, PAGE_READWRITE);
    if (!alloc) {
        logger.Write(LOG_ERROR, "[%s] Failed to alloc %llu bytes.", __FUNCTION__, len);
        return;
    }
    if (!WriteProcessMemory(process, alloc, dll_module.data(), len, 0)) {
        logger.Write(LOG_ERROR, "[%s] WriteProcessMemory Failed", __FUNCTION__);
        return;
    }
    auto thread = CreateRemoteThread(process, 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), alloc, 0, 0);
    if (!thread) {
        logger.Write(LOG_ERROR, "[%s] CreateRemoteThread Failed", __FUNCTION__);
        return;
    }
    WaitForSingleObject(thread, INFINITE);
    VirtualFreeEx(process, alloc, len, MEM_RESERVE);
}

Injector g_Injector;