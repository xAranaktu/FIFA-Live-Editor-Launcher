#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <aclapi.h>
#include <sddl.h>

#include <logger.h>

namespace fs = std::filesystem;

// DLL Injector
class Injector
{
public:
	enum STATUS {
		STATUS_IDLE = 0,
	};


	Injector();
	~Injector();

	bool HasBlacklistedModule(const int pid);
	int GetGamePID();
	void LoadProcNames();
	void LoadBlackList();

	void Inject();

private:
	fs::path dll = "";

	// Procnames we want to attach to
	std::vector<std::string> procnames;

	// Module Blacklist, terminate injection if found
	std::vector<std::string> blacklist;

	bool SetAccessControl(const wchar_t* file, const wchar_t* access);
	bool HasBlacklistedModule(const int pid);
	bool dllExists();
};

extern Injector g_Injector;