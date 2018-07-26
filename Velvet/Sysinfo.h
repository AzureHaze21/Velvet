#pragma once

#include <string>
#include <Winver.h>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <iphlpapi.h>
#include <Winreg.h>
#include <wchar.h>

#pragma comment(lib, "IPHLPAPI.lib")

namespace Sysinfo
{
	static std::string QueryOS()
	{
		HKEY hKey;
		LONG res = RegOpenKeyExW(
			HKEY_LOCAL_MACHINE,
			L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\",
			0, KEY_READ, &hKey);

		char szBuffer[128] = { 0 };
		DWORD dwBufferSize = sizeof(szBuffer);
		ULONG ret = RegQueryValueExA(hKey, "ProductName", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);

		if (ret == ERROR_SUCCESS)
			return std::string(szBuffer);

		return std::string("");
	}

	static std::string QueryMAC()
	{
		IP_ADAPTER_INFO pipInfo[sizeof(IP_ADAPTER_INFO)];
		DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
		DWORD dwStatus = GetAdaptersInfo(pipInfo, &dwBufLen);

		std::stringstream ss;

		for (int b = 0; b < 6; ++b)
			ss << std::hex << std::setfill('0') << std::setw(2) << (int)pipInfo->Address[b] << (b < 5 ? ":" : "");

		return ss.str();
	}

	static std::string QueryIPV4()
	{
		IP_ADAPTER_INFO pipInfo[sizeof(IP_ADAPTER_INFO)];
		DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
		DWORD dwStatus = GetAdaptersInfo(pipInfo, &dwBufLen);
		std::string ret(pipInfo->IpAddressList.IpAddress.String);

		return ret;
	}

	static std::string QueryUsername()
	{
		char buffer[MAX_COMPUTERNAME_LENGTH + 1];
		DWORD dwMaxLen = MAX_COMPUTERNAME_LENGTH;

		if (GetComputerNameA(buffer, &dwMaxLen))
			return std::string(buffer);

		return std::string("");
	}

	static std::string QueryLocale()
	{
		char ISOLang[5] = { 0 };
		char ISOCountry[5] = { 0 };

		GetLocaleInfoA(LOCALE_USER_DEFAULT,
			LOCALE_SISO639LANGNAME,
			ISOLang,
			sizeof(ISOLang));

		GetLocaleInfoA(LOCALE_USER_DEFAULT,
			LOCALE_SISO3166CTRYNAME,
			ISOCountry,
			sizeof(ISOCountry));

		return std::string(ISOLang) + '_' + ISOCountry;
	}
}