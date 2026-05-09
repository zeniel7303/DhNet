#pragma once
#include "GameServer.h"

template<typename T>
inline T* GameServer::GetSystem()
{
    return nullptr;
}

template<>
inline GameSessionSystem* GameServer::GetSystem<GameSessionSystem>()
{
    return m_gameSessionSystem.get();
}

template<>
inline PlayerSystem* GameServer::GetSystem<PlayerSystem>()
{
    return m_playerSystem.get();
}

template<>
inline RoomSystem* GameServer::GetSystem<RoomSystem>()
{
    return m_roomSystem.get();
}

template<>
inline LobbySystem* GameServer::GetSystem<LobbySystem>()
{
    return m_lobbySystem.get();
}

template<>
inline DbSystem* GameServer::GetSystem<DbSystem>()
{
    return m_dbSystem.get();
}
