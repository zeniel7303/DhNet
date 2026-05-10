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

	void Init(const char* username, const char* password)
	{
		PacketHeader::Init(PacketEnum::Req_Login, sizeof(*this));
		strncpy_s(m_username, username, sizeof(m_username) - 1);
		m_username[sizeof(m_username) - 1] = '\0';
		strncpy_s(m_password, password, sizeof(m_password) - 1);
		m_password[sizeof(m_password) - 1] = '\0';
	}

	char m_username[16];
	char m_password[64];	// plaintext; AES-GCM encryption will be added in a later phase
};

class ResLoginFailed : public PacketHeader
{
public:
	ResLoginFailed() = default;
	~ResLoginFailed() = default;

	void Init()
	{
		PacketHeader::Init(PacketEnum::Res_LoginFailed, sizeof(*this));
	}
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

	void Init(const char* message)
	{
		PacketHeader::Init(PacketEnum::Req_RoomChat, sizeof(*this));
		strncpy_s(m_message, message, sizeof(m_message) - 1);
		m_message[sizeof(m_message) - 1] = '\0';
	}

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

struct PlayerInfo
{
	uint64 m_playerId;
	char m_playerName[16];
};

struct RoomInfo
{
	int32 m_roomIndex;
	int32 m_playerCount;
	int32 m_maxPlayers;
};

class ResLobbyEnter : public PacketHeader
{
public:
	ResLobbyEnter() = default;
	~ResLobbyEnter() = default;

	void Init(int32 _lobbyIndex)
	{
		PacketHeader::Init(PacketEnum::Res_LobbyEnter, sizeof(*this));
		m_lobbyIndex = _lobbyIndex;
		m_playerCount = 0;
		memset(m_players, 0, sizeof(m_players));
	}

	void AddPlayer(uint64 _playerId, const char* _playerName)
	{
		if (m_playerCount >= 50) return;
		m_players[m_playerCount].m_playerId = _playerId;
		strncpy_s(m_players[m_playerCount].m_playerName, _playerName, sizeof(PlayerInfo::m_playerName) - 1);
		m_players[m_playerCount].m_playerName[sizeof(PlayerInfo::m_playerName) - 1] = '\0';
		m_playerCount++;
	}

	int32 m_lobbyIndex;
	int32 m_playerCount;
	PlayerInfo m_players[50];
};

class NotiLobbyPlayerEnter : public PacketHeader
{
public:
	NotiLobbyPlayerEnter() = default;
	~NotiLobbyPlayerEnter() = default;

	void Init(uint64 _playerId, std::string _name)
	{
		PacketHeader::Init(PacketEnum::Noti_LobbyPlayerEnter, sizeof(*this));
		m_playerId = _playerId;
		strncpy_s(m_playerName, _name.c_str(), sizeof(m_playerName) - 1);
		m_playerName[sizeof(m_playerName) - 1] = '\0';
	}

	uint64 m_playerId;
	char m_playerName[16];
};

class NotiLobbyPlayerExit : public PacketHeader
{
public:
	NotiLobbyPlayerExit() = default;
	~NotiLobbyPlayerExit() = default;

	void Init(uint64 _playerId, std::string _name)
	{
		PacketHeader::Init(PacketEnum::Noti_LobbyPlayerExit, sizeof(*this));
		m_playerId = _playerId;
		strncpy_s(m_playerName, _name.c_str(), sizeof(m_playerName) - 1);
		m_playerName[sizeof(m_playerName) - 1] = '\0';
	}

	uint64 m_playerId;
	char m_playerName[16];
};

class ReqLobbyChat : public PacketHeader
{
public:
	ReqLobbyChat() = default;
	~ReqLobbyChat() = default;

	void Init(const char* message)
	{
		PacketHeader::Init(PacketEnum::Req_LobbyChat, sizeof(*this));
		strncpy_s(m_message, message, sizeof(m_message) - 1);
		m_message[sizeof(m_message) - 1] = '\0';
	}

	char m_message[256];
};

class NotiLobbyChat : public PacketHeader
{
public:
	NotiLobbyChat() = default;
	~NotiLobbyChat() = default;

	void Init(uint64 _playerId, std::string _name, const char* _message)
	{
		PacketHeader::Init(PacketEnum::Noti_LobbyChat, sizeof(*this));
		m_playerId = _playerId;
		strncpy_s(m_playerName, _name.c_str(), sizeof(m_playerName) - 1);
		m_playerName[sizeof(m_playerName) - 1] = '\0';
		strncpy_s(m_message, _message, sizeof(m_message) - 1);
		m_message[sizeof(m_message) - 1] = '\0';
	}

	uint64 m_playerId;
	char m_playerName[16];
	char m_message[256];
};

class ReqRoomList : public PacketHeader
{
public:
	ReqRoomList() = default;
	~ReqRoomList() = default;
};

class ResRoomList : public PacketHeader
{
public:
	ResRoomList() = default;
	~ResRoomList() = default;

	void Init()
	{
		PacketHeader::Init(PacketEnum::Res_RoomList, sizeof(*this));
		m_roomCount = 0;
		memset(m_rooms, 0, sizeof(m_rooms));
	}

	void AddRoom(int32 _roomIndex, int32 _playerCount, int32 _maxPlayers)
	{
		if (m_roomCount >= 20) return;
		m_rooms[m_roomCount] = { _roomIndex, _playerCount, _maxPlayers };
		m_roomCount++;
	}

	int32 m_roomCount;
	RoomInfo m_rooms[20];
};

#pragma pack(pop)