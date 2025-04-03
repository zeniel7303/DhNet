#pragma once
#include "../ServerCore/Sender.h"

class GameSession;

class GameSessionManager
{
private:
	USE_LOCK;
	std::set<std::shared_ptr<GameSession>> m_sessions;

public:
	void Add(std::shared_ptr<GameSession> _session);
	void Remove(std::shared_ptr<GameSession> _session);
	void Broadcast(std::shared_ptr<Sender> _sender);
};

extern GameSessionManager GSessionManager;
