#pragma once

#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#elif _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32")
#endif

#ifndef sockaddr_in
#define sockaddr_in struct sockaddr_in 
#endif

#include <string>

#include "PacketHandler.h"

class TCPConnection
{
#define MAX_BUF 1024
#define RECV_ERROR ((std::size_t)(~0))

private:
	int m_socket;
	sockaddr_in m_addr;
	std::string m_ip;
	uint16_t m_port;
	PacketHandler m_packetHandler;
	byte buffer[MAX_BUF];
	std::size_t m_pos{ 0 };
	bool m_isValid;

public:
	explicit TCPConnection(const char *ip, uint16_t port) :
		m_ip(ip), m_port(port) 
	{
		Close();  Connect(ip, port);
	}

	virtual ~TCPConnection() { Close(); }
	
	bool Connect(const char *ip, uint16_t port);
	bool Retry() { return Connect(m_ip.c_str(), m_port); }
	void Close();

	std::size_t ReadStream();
	std::size_t Send(const byte *data, std::size_t size);
	std::size_t SendPacket(_packet p);

	std::vector<_packet> GetPackets();
	int GetSocket() const { return m_socket; }
	bool Valid() { return m_isValid; }
	operator bool() { return Valid(); }
};