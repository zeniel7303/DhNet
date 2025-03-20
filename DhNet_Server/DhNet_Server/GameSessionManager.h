#pragma once
#include "../ServerCore/SendBuffer.h"

class GameSession;

class GameSessionManager
{
private:
	USE_LOCK;
	set<shared_ptr<GameSession>> m_sessions;

public:
	void Add(shared_ptr<GameSession> _session);
	void Remove(shared_ptr<GameSession> _session);
	void Broadcast(shared_ptr<SendBuffer> _sendBuffer);

	shared_ptr<GameSession> CreateSession();
};

extern GameSessionManager GSessionManager;
