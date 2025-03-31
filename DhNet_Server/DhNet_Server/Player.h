#pragma once

class GameSession;

class Player
{
private:
	uint64						m_playerId = 0;
	string						m_name;
	shared_ptr<GameSession>		m_ownerSession; // ��ȯ���� ����

public:
	Player(uint64 _id, string _name, shared_ptr<GameSession> _session);
	~Player();

	uint64 GetPlayerId() { return m_playerId; }
	shared_ptr<GameSession> GetOwnerSession() { return m_ownerSession; }
};