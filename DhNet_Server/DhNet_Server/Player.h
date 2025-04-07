#pragma once

class GameSession;

class Player : public std::enable_shared_from_this<Player>
{
private:
	uint64							m_playerId = 0;
	std::string						m_name;
	std::shared_ptr<GameSession>	m_ownerSession; // ��ȯ���� ����

public:
	Player(std::shared_ptr<GameSession> _session);
	~Player();

	void LeaveRoom();

	uint64 GetPlayerId()							{ return m_playerId; }
	std::string GetPlayerName()						{ return m_name; }
	std::shared_ptr<GameSession> GetOwnerSession()	{ return m_ownerSession; }
};