#pragma once

class GameSession;
class Room;
class Lobby;

class Player : public std::enable_shared_from_this<Player>
{
private:
	uint64							m_playerId = 0;
	std::string						m_name;
	std::shared_ptr<GameSession>	m_ownerSession;
	std::weak_ptr<Room>				m_currentRoom;
	std::weak_ptr<Lobby>			m_currentLobby;

public:
	Player(std::shared_ptr<GameSession> _session, uint64 _accountId, std::string _name);
	~Player();

	void EnterRoom();
	void EnterRoomFailed();
	void LeaveRoom();
	void LeaveRoomAndReenterLobby();
	void LeaveRoomFailed();
	void RoomChat(std::string _message);

	void LeaveLobby();
	void LobbyChat(std::string _message);

	uint64 GetPlayerId()							{ return m_playerId; }
	std::string GetPlayerName()						{ return m_name; }
	std::shared_ptr<GameSession> GetOwnerSession()	{ return m_ownerSession; }
	std::weak_ptr<Room> GetCurrentRoom()			{ return m_currentRoom; }
	std::weak_ptr<Lobby> GetCurrentLobby()			{ return m_currentLobby; }

	void SetCurrentRoom(const std::shared_ptr<Room>& _room)   { m_currentRoom = _room; }
	void SetCurrentLobby(const std::shared_ptr<Lobby>& _lobby) { m_currentLobby = _lobby; }
};