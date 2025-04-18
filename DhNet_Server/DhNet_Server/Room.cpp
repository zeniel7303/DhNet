#include "stdafx.h"
#include "Room.h"

void Room::Enter(std::shared_ptr<Player> _player)
{
	m_players[_player->GetPlayerId()] = _player;
}

void Room::Leave(std::shared_ptr<Player> _player)
{
	auto it = m_players.find(_player->GetPlayerId());
	if (it != m_players.end())
	{
		m_players.erase(it);
	}
}

void Room::Broadcast(std::shared_ptr<Sender> _sender)
{
	for (auto& p : m_players)
	{
		p.second->GetOwnerSession()->Send(_sender);
	}
}