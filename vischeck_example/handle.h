#pragma once
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <exception>
#include <psapi.h>
#include <mutex>
#include "offsets.h"

class Memory {
private:
	std::mutex m;
	DWORD PID;
	HANDLE csgo;
	uint64_t client_base;
public:
	void open(const TCHAR* processName) {
		PID = GetPID("cs2.exe");
		csgo = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
		client_base = (uint64_t)GetModuleBase("client.dll", csgo);
	}

	uint64_t get_client_base() {
		return client_base;
	}

	LONG_PTR GetPID(const TCHAR* processName);
	LONG_PTR GetModuleBase(const TCHAR* moduleName, HANDLE handle);

	template <typename T>
	T Read(uint64_t address);
};

template <typename T>
inline T Memory::Read(uint64_t address)
{
	std::lock_guard<std::mutex> l(m);
	T out;
	ReadProcessMemory(csgo, (void*)(address), (void*)&out, sizeof(T), NULL);
	return out;
}
