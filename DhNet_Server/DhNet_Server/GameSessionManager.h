#pragma once
#include "../ServerCore/Sender.h"

class GameSession;

class GameSessionManager
{
private:
	USE_LOCK;
	set<shared_ptr<GameSession>> m_sessions;

public:
	void Add(shared_ptr<GameSession> _session);
	void Remove(shared_ptr<GameSession> _session);
	void Broadcast(shared_ptr<Sender> _sender);
};

extern GameSessionManager GSessionManager;
