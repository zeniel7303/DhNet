#pragma once
#include "../ServerCore/PacketHeader.h"
#include "PacketEnum.h"

#pragma pack(push, 1)

class TestPacket : public PacketHeader
{
public:
	TestPacket() = default;
	~TestPacket() = default;

	char m_test[256];
};

#pragma pack(pop)