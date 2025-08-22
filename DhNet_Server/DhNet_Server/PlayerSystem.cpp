#include "stdafx.h"
#include "Player.h"
#include "PlayerSystem.h"

PlayerSystem::~PlayerSystem()
{
	m_players.clear();
}

void PlayerSystem::Add(std::shared_ptr<Player> _player)
{
	m_players.insert(std::make_pair(_player->GetPlayerId(), _player));
}

void PlayerSystem::Remove(std::shared_ptr<Player> _player)
{
	WRITE_LOCK
	m_players.unsafe_erase(_player->GetPlayerId());
}

std::shared_ptr<Player> PlayerSystem::Find(int _id)
{
	if (const auto it = m_players.find(_id); it != m_players.end())
	{
		return it->second;
	}
	return nullptr;
}