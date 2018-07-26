#include "Injector.h"

namespace
{
	DWORD GetPID(const wchar_t *szName)
	{
		PROCESSENTRY32 entry;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		entry.dwSize = sizeof(PROCESSENTRY32);

		if (hSnap != NULL)
		{
			Process32First(hSnap, &entry);

			do
			{
				if (!lstrcmp(entry.szExeFile, szName))
					return (entry.th32ProcessID);
			} while (Process32Next(hSnap, &entry));
		}

		return 0;
	}
}

bool Injector::TryInject(DWORD dwPid, PCHAR dllPath)
{
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProc != NULL)
	{
		PVOID pAlloc = VirtualAllocEx(hProc, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
		if (pAlloc != NULL)
		{
			if (WriteProcessMemory(hProc, pAlloc, dllPath, strlen(dllPath) + 1, NULL))
			{
				HMODULE hKernel32 = GetModuleHandle(L"Kernel32.dll");
				if (hKernel32 != NULL)
				{
					HANDLE pLoadLibrary = GetProcAddress(hKernel32, "LoadLibraryA");
					if (pLoadLibrary)
					{
						HANDLE ret = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pAlloc, 0, NULL);

						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Injector::TryInject(const wchar_t *szProcName, PCHAR dllPath)
{
	DWORD pid = GetPID(szProcName);

	return !pid ? false : TryInject(pid, dllPath);
}

void Injector::InjectRange(DWORD dwPid, PCHAR dllPath, int start, int end)
{
	for (int i = start; i < end; i++)
		TryInject(dwPid, dllPath);
}

void Injector::InjectAll(PCHAR dllPath)
{
	PROCESSENTRY32 entry;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	entry.dwSize = sizeof(PROCESSENTRY32);

	if (hSnap != NULL)
	{
		Process32First(hSnap, &entry);

		do
		{
			TryInject(entry.th32ProcessID, dllPath);
		} while (Process32Next(hSnap, &entry));
	}
}