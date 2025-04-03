#include "stdafx.h"
#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager GSessionManager;

void GameSessionManager::Add(std::shared_ptr<GameSession> _session)
{
	WRITE_LOCK;
	m_sessions.insert(_session);
}

void GameSessionManager::Remove(std::shared_ptr<GameSession> _session)
{
	WRITE_LOCK;
	m_sessions.erase(_session);
}

void GameSessionManager::Broadcast(std::shared_ptr<Sender> _sender)
{
	WRITE_LOCK;
	for (std::shared_ptr<GameSession> session : m_sessions)
	{
		session->Send(_sender);
	}
}