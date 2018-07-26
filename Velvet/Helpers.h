#pragma once

#include "BinaryUtils.h"
#include "FileUtils.h"

#include <Windows.h>
#include <cscapi.h>

static bool add_reg_key(LPCWSTR value, LPCWSTR keyName)
{
	HKEY hKey;
	LSTATUS ret;

	ret = RegOpenKeyEx(
		HKEY_CURRENT_USER, 
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
		0, KEY_QUERY_VALUE | KEY_READ | KEY_WRITE, &hKey);

	if (ret != 0)
		return false;

	BYTE buf[256] = { 0 };
	memcpy(buf, value, sizeof(buf));

	ret = RegSetValueEx(hKey, keyName, 0, REG_SZ, (const BYTE*)buf, sizeof(buf));
	if (ret != 0)
	{
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);

	return true;
}

static bool delete_reg_key(LPCWSTR keyName)
{
	HKEY hKey;
	LSTATUS ret;

	ret = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, KEY_QUERY_VALUE | KEY_READ | KEY_WRITE, &hKey);

	if (ret != 0)
		return false;

	ret = RegDeleteValue(hKey, keyName);
	if (ret != 0)
	{
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	return true;
}

static bool change_mutex()
{
	GUID guid;
	WCHAR* guidStr;
	HRESULT hGuid;
	char buf[38 + 1] = { 0 };
	std::size_t len = sizeof(buf);
	
	hGuid = CoCreateGuid(&guid);
	StringFromCLSID(guid, &guidStr);
	wcstombs_s(&len, buf, guidStr, len);

	TCHAR szExePath[MAX_PATH];
	GetModuleFileName(NULL, szExePath, MAX_PATH);

	auto bytes = File::GetAllBytes(szExePath);
	size_t offset = BinaryUtils::sigScan(bytes.data(), bytes.size(),
		reinterpret_cast<const byte*>("{11111111-1111-1111-1111-111111111111}"),
		reinterpret_cast<const byte*>("x????????x????x????x????x????????????x"));

	if (offset > 0)
	{
		memcpy(&bytes[offset], buf, sizeof(buf));
		return File::WriteAllBytes(bytes.data(), bytes.size(), szExePath) > 0;
	}

	return false;
}