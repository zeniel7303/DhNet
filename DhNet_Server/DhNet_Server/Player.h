#pragma once

class GameSession;

class Player : public std::enable_shared_from_this<Player>
{
private:
	uint64							m_playerId = 0;
	std::string						m_name;
	std::shared_ptr<GameSession>	m_ownerSession; // 순환참조 주의

public:
	Player(uint64 _id, std::string _name, std::shared_ptr<GameSession> _session);
	~Player();

	void LeaveRoom();

	uint64 GetPlayerId()							{ return m_playerId; }
	std::string GetPlayerName()						{ return m_name; }
	std::shared_ptr<GameSession> GetOwnerSession()	{ return m_ownerSession; }
};