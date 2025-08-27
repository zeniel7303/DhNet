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

   	void Init(uint64 _playerId, std::string _name)
    {
		PacketHeader::Init(PacketEnum::Res_Login, sizeof(*this));
		m_playerId = _playerId;
		strncpy_s(m_playerName, _name.c_str(), sizeof(m_playerName) - 1);
		m_playerName[sizeof(m_playerName) - 1] = '\0'; // Ensure null-termination
    }

	uint64 m_playerId;
	char m_playerName[16];
};

class ReqRoomEnter : public PacketHeader
{
public:
	ReqRoomEnter() = default;
	~ReqRoomEnter() = default;

	void Init()
	{
		PacketHeader::Init(PacketEnum::Req_RoomEnter, sizeof(*this));
	}
};

class ResRoomEnter : public PacketHeader
{
public:
	ResRoomEnter() = default;
	~ResRoomEnter() = default;

	void Init(bool _isSuccess)
	{
		PacketHeader::Init(PacketEnum::Res_RoomEnter, sizeof(*this));
		m_isSuccess = _isSuccess;
	}

	bool m_isSuccess;
};

class NotiRoomEnter : public PacketHeader
{
public:
	NotiRoomEnter() = default;
	~NotiRoomEnter() = default;

	void Init(uint64 _playerId, std::string _name)
	{
		PacketHeader::Init(PacketEnum::Noti_RoomEnter, sizeof(*this));
		m_playerId = _playerId;
		strncpy_s(m_playerName, _name.c_str(), sizeof(m_playerName) - 1);
		m_playerName[sizeof(m_playerName) - 1] = '\0'; // Ensure null-termination
	}

	uint64 m_playerId;
	char m_playerName[16];
};

class ReqRoomChat : public PacketHeader
{
public:
	ReqRoomChat() = default;
	~ReqRoomChat() = default;

	char m_message[256];
};

class NotiRoomChat : public PacketHeader
{
public:
	NotiRoomChat() = default;
	~NotiRoomChat() = default;

    void Init(uint64 _playerId, std::string _name, const char* _message)
    {
		PacketHeader::Init(PacketEnum::Noti_RoomChat, sizeof(*this));
		m_playerId = _playerId;
		strncpy_s(m_playerName, _name.c_str(), sizeof(m_playerName) - 1);
		m_playerName[sizeof(m_playerName) - 1] = '\0'; // Ensure null-termination
		strncpy_s(m_message, _message, sizeof(m_message) - 1);
		m_message[sizeof(m_message) - 1] = '\0'; // Ensure null-termination
    }

	uint64 m_playerId;
	char m_playerName[16];
	char m_message[256];
};

class ReqRoomExit : public PacketHeader
{
public:
	ReqRoomExit() = default;
	~ReqRoomExit() = default;
};

class ResRoomExit : public PacketHeader
{
public:
	ResRoomExit() = default;
	~ResRoomExit() = default;

	void Init(bool _isSuccess)
	{
		PacketHeader::Init(PacketEnum::Res_RoomExit, sizeof(*this));
		m_isSuccess = _isSuccess;
	}

	bool m_isSuccess;
};

class NotiRoomExit : public PacketHeader
{
public:
	NotiRoomExit() = default;
	~NotiRoomExit() = default;

	void Init(uint64 _playerId, std::string _name)
	{
		PacketHeader::Init(PacketEnum::Noti_RoomExit, sizeof(*this));
		m_playerId = _playerId;
		strncpy_s(m_playerName, _name.c_str(), sizeof(m_playerName) - 1);
		m_playerName[sizeof(m_playerName) - 1] = '\0'; // Ensure null-termination
	}

	uint64 m_playerId;
	char m_playerName[16];
};

#pragma pack(pop)