#include "stdafx.h"
#include "Player.h"
#include "PlayerSystem.h"

PlayerSystem::~PlayerSystem()
{
	m_players.clear();
}

void PlayerSystem::Add(std::shared_ptr<Player> _player)
{
	WRITE_LOCK
	m_players.insert(std::make_pair(_player->GetPlayerId(), _player));
}

void PlayerSystem::Remove(std::shared_ptr<Player> _player)
{
	WRITE_LOCK
	m_players.erase(_player->GetPlayerId());
}

std::shared_ptr<Player> PlayerSystem::Find(uint64 _id)
{
	READ_LOCK
	if (const auto it = m_players.find(_id); it != m_players.end())
	{
		return it->second;
	}
	return nullptr;
}

std::vector<std::shared_ptr<Player>> PlayerSystem::GetPlayers()
{
	READ_LOCK
	std::vector<std::shared_ptr<Player>> result;
	result.reserve(m_players.size());
	for (const auto& kv : m_players)
		result.push_back(kv.second);
	return result;
}