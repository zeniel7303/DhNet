#pragma once

class GameSession;

class Player : public enable_shared_from_this<Player>
{
private:
	uint64						m_playerId = 0;
	string						m_name;
	shared_ptr<GameSession>		m_ownerSession; // 순환참조 주의

public:
	Player(uint64 _id, string _name, shared_ptr<GameSession> _session);
	~Player();

	void LeaveRoom();

	uint64 GetPlayerId() { return m_playerId; }
	string GetPlayerName() { return m_name; }
	shared_ptr<GameSession> GetOwnerSession() { return m_ownerSession; }
};