#include "stdafx.h"
#include "LobbySystem.h"
#include "../../DhUtil/ObjectPool.h"

LobbySystem::LobbySystem()
{
	for (int32 i = 0; i < MAX_LOBBY_COUNT; i++)
	{
		auto lobby = ObjectPool<Lobby>::MakeShared();
		lobby->SetLobbyIndex(i);
		m_lobbies.push_back(lobby);
	}
}

std::shared_ptr<Lobby> LobbySystem::AssignLobby()
{
	READ_LOCK

	// 클러스터링: 인원 가장 많은 로비 우선 시도
	std::shared_ptr<Lobby> best = nullptr;
	int32 bestCount = -1;
	for (auto& lobby : m_lobbies)
	{
		int32 cnt = lobby->GetPlayerCount();
		if (cnt < MAX_LOBBY_PLAYERS && cnt > bestCount)
		{
			best = lobby;
			bestCount = cnt;
		}
	}

	if (best && best->TryReserveSlot())
		return best;

	// 우선 로비가 방금 만원이 됐으면 다른 로비에서 슬롯 확보
	for (auto& lobby : m_lobbies)
	{
		if (lobby && lobby->TryReserveSlot())
			return lobby;
	}

	return nullptr;
}
