#pragma once
#include "GameServer.h"

template<typename T>
inline T* GameServer::GetSystem()
{
    return nullptr; // 기본 템플릿은 nullptr
}

template<>
inline UniqueIdGenerationSystem* GameServer::GetSystem<UniqueIdGenerationSystem>()
{
    return m_uniqueIdGenerationSystem;
}

template<>
inline GameSessionSystem* GameServer::GetSystem<GameSessionSystem>()
{
    return m_gameSessionSystem;
}

template<>
inline PlayerSystem* GameServer::GetSystem<PlayerSystem>()
{
    return m_playerSystem;
}

template<>
inline LobbySystem* GameServer::GetSystem<LobbySystem>()
{
	return m_lobbySystem;
}