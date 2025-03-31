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

class ReqLogin : public PacketHeader
{
public:
	ReqLogin() = default;
	~ReqLogin() = default;
};

class ResLogin : public PacketHeader
{
public:
	ResLogin() = default;
	~ResLogin() = default;

	bool m_isSuccess;
};

class ReqRoomEnter : public PacketHeader
{
public:
	ReqRoomEnter() = default;
	~ReqRoomEnter() = default;
};

class ResRoomEnter : public PacketHeader
{
public:
	ResRoomEnter() = default;
	~ResRoomEnter() = default;

	bool m_isSuccess;
};

#pragma pack(pop)