#include "handle.h"

LONG_PTR Memory::GetPID(const TCHAR* processName)
{
	PROCESSENTRY32 pe = { sizeof(pe) };
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == NULL)
		throw std::exception("¥¥Ω®øÏ’’ ß∞‹");
	bool success;
	success = Process32First(snapshot, &pe);
	if (_tcscmp(processName, pe.szExeFile) == 0)
	{
		CloseHandle(snapshot);
		return pe.th32ProcessID;
	}
	while (success)
	{
		success = Process32Next(snapshot, &pe);
		if (_tcscmp(processName, pe.szExeFile) == 0)
		{
			CloseHandle(snapshot);
			return pe.th32ProcessID;
		}
	}
	return NULL;
}

LONG_PTR Memory::GetModuleBase(const TCHAR* moduleName, HANDLE handle)
{
	HMODULE hmodules[1024];
	DWORD size;
	EnumProcessModules(handle, hmodules, sizeof(hmodules), &size);
	for (size_t i = 0; i < size; i++)
	{
		HMODULE mod = hmodules[i];
		TCHAR name[MAX_PATH];
		GetModuleBaseName(handle, mod, name, MAX_PATH);
		if (_tcscmp(moduleName, name) == 0)
		{
			return (LONG_PTR)mod;
		}
	}
	return NULL;
}