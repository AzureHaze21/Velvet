#include <iostream>

#include "Connection.h"

namespace
{
	std::string GetLastErrorMsg()
	{
		char buf[256] = { 0 };
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buf, (sizeof(buf) / sizeof(wchar_t)), NULL);

		return std::string(buf);
	}	
}

bool TCPConnection::Connect(const char * ip, uint16_t port)
{
#ifdef _WIN32 // init WinSock
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (ret != NO_ERROR)
		goto error;
#endif

	sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(PF_INET, ip, &(addr.sin_addr));

#ifdef _WIN32
	m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	if (m_socket == INVALID_SOCKET)
		goto error;
#elif __linux__
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == -1)
		goto error;
#endif

	ret = connect(m_socket, (sockaddr*)&addr, sizeof(addr));
	if (ret != 0)
		goto error;
	else
		m_isValid = true;

	return true;

error:
#ifdef _DEBUG
	printf("Failed to connect to %s. %s", ip, GetLastErrorMsg().c_str());
#endif
	Close();
	return false;
}

void TCPConnection::Close()
{
#ifdef _WIN32
	closesocket(m_socket);
	WSACleanup();
#elif __linux__
	close(socket);
#endif
	m_socket = -1;
	m_packetHandler.Reset();
	m_isValid = false;
}

std::size_t TCPConnection::ReadStream()
{
	std::size_t bytesRead = recv((SOCKET)m_socket, (char*)(buffer + m_pos), sizeof(buffer) - m_pos, 0);
	m_pos = m_packetHandler.ReadPackets(buffer, m_pos + bytesRead);

	return bytesRead;
}

std::size_t TCPConnection::Send(const byte *data, std::size_t size)
{
	return send(m_socket, (const char*)data, size, 0);
}

std::size_t TCPConnection::SendPacket(_packet p)
{
	std::vector<byte>  tmp(p.header.size);

	memcpy(&tmp.data()[0], &p.header, sizeof(_header));
	memcpy(&tmp.data()[sizeof(_header)], p.data.data(), p.data.size());

	return send(m_socket, (const char*)tmp.data(), tmp.size(), 0);
}

std::vector<_packet> TCPConnection::GetPackets()
{
	std::vector<_packet> tmp;
	std::swap(tmp, m_packetHandler.packets);
	return tmp;
}
