#pragma once

#include <Windows.h>
#include <TlHelp32.h>

class Injector
{
public:
	static bool TryInject(DWORD dwPid, PCHAR dllPath);
	static bool TryInject(const wchar_t *szProcName, PCHAR dllPath);
	static void InjectRange(DWORD dwPid, PCHAR dllPath, int start, int end);
	static void InjectAll(PCHAR dllPath);
};