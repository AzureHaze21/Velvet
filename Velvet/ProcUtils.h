#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <iostream>

namespace ProcUtils
{
	static std::vector<PROCESSENTRY32> GetProcList()
	{
		PROCESSENTRY32 pe32;

		ZeroMemory(&pe32, sizeof(pe32));
		pe32.dwSize = sizeof(pe32);

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		
		if (!Process32First(hSnapshot, &pe32))
			return std::vector<PROCESSENTRY32>(0);

		std::vector<PROCESSENTRY32> procList;

		do
		{
			procList.push_back(pe32);
		} while (Process32Next(hSnapshot, &pe32));
	
		return procList;
	}

	static std::string GetProcNameByHwnd(HWND hwnd)
	{
		PROCESSENTRY32 pe32;

		ZeroMemory(&pe32, sizeof(pe32));
		pe32.dwSize = sizeof(pe32);

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (!Process32First(hSnapshot, &pe32))
			return std::string("");

		DWORD pid;
		GetWindowThreadProcessId(hwnd, &pid);

		do
		{
			if (pe32.th32ProcessID == pid)
			{
				char buffer[260]{ 0 };
				if (wcstombs(buffer, pe32.szExeFile, sizeof(buffer)) > 0)	
					return std::string(buffer);
				return std::string("");
			}
			
		} while (Process32Next(hSnapshot, &pe32));

		return std::string("");
	}
}
