#pragma once

class GameSession;
class Room;

class Player : public std::enable_shared_from_this<Player>
{
private:
	uint64							m_playerId = 0;
	std::string						m_name;
	std::shared_ptr<GameSession>	m_ownerSession; // ��ȯ���� ����
	std::weak_ptr<Room>				m_currentRoom; // ��ȯ���� ����: Player�� Room�� ���� �����θ� ����

public:
	Player(std::shared_ptr<GameSession> _session);
	~Player();

	void EnterRoom();
	void EnterRoomFailed();
	void LeaveRoom();
	void LeaveRoomFailed();
	void RoomChat(std::string _message);

	uint64 GetPlayerId()							{ return m_playerId; }
	std::string GetPlayerName()						{ return m_name; }
	std::shared_ptr<GameSession> GetOwnerSession()	{ return m_ownerSession; }
	std::weak_ptr<Room> GetCurrentRoom()			{ return m_currentRoom; }

	// ���� �� ���� (���� ������ ����)
	void SetCurrentRoom(const std::shared_ptr<Room>& room) { m_currentRoom = room; }
};