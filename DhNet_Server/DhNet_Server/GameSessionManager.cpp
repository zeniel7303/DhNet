#include "stdafx.h"
#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager GSessionManager;

void GameSessionManager::Add(shared_ptr<GameSession> _session)
{
	WRITE_LOCK;
	m_sessions.insert(_session);
}

void GameSessionManager::Remove(shared_ptr<GameSession> _session)
{
	WRITE_LOCK;
	m_sessions.erase(_session);
}

void GameSessionManager::Broadcast()
{
	WRITE_LOCK;
}

shared_ptr<GameSession> GameSessionManager::CreateSession()
{
	return make_shared<GameSession>();
}