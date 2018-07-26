#include "PacketHandler.h"
#include "crc32.h"

bool PacketHandler::CheckPacketCRC32(_packet *packet)
{
	uint32_t check = CRC32::crc32(packet->data.data(), packet->data.size());

	return (packet->header.crc32 == check);
}

void PacketHandler::ReadNextPacket(byte *buffer)
{
	_packet packet;
	size_t packetSize = ((_header*)(buffer))->size;

	memcpy(&packet.header, buffer, sizeof(_header));

	if (packetSize > sizeof(_header))
	{
		packet.data.resize(packetSize - sizeof(_header));
		memcpy(&packet.data[0], buffer + sizeof(_header), packetSize - sizeof(_header));
	}

	if (packet.header.opcode <= MAX_OPCODE &&
		packets.size() < maxQueueSize)
	{
		packets.push_back(packet);
	}
}

size_t PacketHandler::ReadPackets(byte *data, size_t size)
{
	size_t total = size;

	if (size == ~0) return 0;

	while (size > 0)
	{
		size_t i = 0;
		while (i < size) {
			if (!memcmp(&data[i], "SYN", 3)) {
				if (i > 0)
					memmove(data, data + i, size - i);
				size -= i;
				break;
			}
			++i;
		}

		if (i == size) return 0;
		if (size < sizeof(_header)) return size;

		size_t packetSize = ((_header*)(data))->size;

		if (packetSize <= size) 
		{
			ReadNextPacket(data);
			memmove(data, data + packetSize, size - packetSize);
			size -= packetSize;
		}
		else
			return ((size < total) ? size : 0);
	}

	return 0;
}