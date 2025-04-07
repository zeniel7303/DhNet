#pragma once
#include "../ServerCore/Sender.h"

class GameSession;

class GameSessionSystem
{
private:
	USE_LOCK;
	// std::set<std::shared_ptr<GameSession>> m_sessions;
	concurrency::concurrent_unordered_set<std::shared_ptr<GameSession>> m_sessions;

public:
	GameSessionSystem() = default;
	~GameSessionSystem();

	void Add(std::shared_ptr<GameSession> _session);
	void Remove(std::shared_ptr<GameSession> _session);
	void Broadcast(std::shared_ptr<Sender> _sender);
};