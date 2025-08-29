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
	WRITE_LOCK
	m_sessions.unsafe_erase(_session);
}