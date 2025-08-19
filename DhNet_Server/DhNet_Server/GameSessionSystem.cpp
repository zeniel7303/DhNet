#include "stdafx.h"
#include "GameSessionSystem.h"
#include "GameSession.h"
#include <vector>

GameSessionSystem::~GameSessionSystem()
{
	for (std::shared_ptr<GameSession> session : m_sessions)
	{
		session->Disconnect(L"GameSessionManager Destructor");
	}
	m_sessions.clear();
}

void GameSessionSystem::Add(std::shared_ptr<GameSession> _session)
{
	m_sessions.insert(_session);
}

void GameSessionSystem::Remove(std::shared_ptr<GameSession> _session)
{
	WRITE_LOCK;
	m_sessions.unsafe_erase(_session);
}

void GameSessionSystem::Broadcast(std::shared_ptr<Sender> _sender)
{
	std::vector<std::shared_ptr<GameSession>> snapshot;
	{
		WRITE_LOCK;
		snapshot.reserve(m_sessions.size());
		for (const auto& session : m_sessions)
		{
			snapshot.push_back(session);
		}
	}
	for (const auto& session : snapshot)
	{
		session->Send(_sender);
	}
}