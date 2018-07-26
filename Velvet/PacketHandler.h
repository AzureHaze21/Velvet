#pragma once

#include <vector>

#include "Packet.h"

#define DEFAULT_QUEUE_SIZE 64

class PacketHandler
{
	friend class TCPConnection;

private:
	std::size_t maxQueueSize{ 0 };
	std::vector<_packet> packets;
private:
	void ReadNextPacket(byte *buffer);
	bool CheckPacketCRC32(_packet *packet);
public:
	PacketHandler() : maxQueueSize(DEFAULT_QUEUE_SIZE) {}
	PacketHandler(std::size_t queueSize) : maxQueueSize(queueSize) {};
	virtual ~PacketHandler() {}

	std::size_t ReadPackets(byte *data, size_t size);
	//const std::vector<_packet>& GetPacketQueue() const { return packets; }
	void SetQueueSize(std::size_t newSize) { maxQueueSize = newSize; }
	void Reset() { decltype(packets) tmp; packets.swap(tmp); }
};