#pragma once

#pragma pack(push, 1)

class PacketHeader
{
public:
	unsigned short m_packetNum;
	unsigned short m_dataSize;
};

#pragma pack(pop)