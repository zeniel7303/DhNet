#include "stdafx.h"
#include "Player.h"
#include "PlayerSystem.h"

PlayerSystem::~PlayerSystem()
{
	WRITE_LOCK;
	for (auto it = m_players.begin(); it != m_players.end(); ++it)
	{
		it->second->~Player();
	}

	m_players.clear();
}

void PlayerSystem::Add(std::shared_ptr<Player> _player)
{
	m_players.insert(std::make_pair(_player->GetPlayerId(), _player));
}

void PlayerSystem::Remove(std::shared_ptr<Player> _player)
{
	WRITE_LOCK;
	m_players.unsafe_erase(_player->GetPlayerId());
}

std::shared_ptr<Player> PlayerSystem::Find(int _id)
{
	auto it = m_players.find(_id);
	if (it != m_players.end())
	{
		return it->second;
	}
	return nullptr;
}