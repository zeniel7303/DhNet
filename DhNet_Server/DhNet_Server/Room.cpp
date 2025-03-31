#include "stdafx.h"
#include "Room.h"

Room GRoom;

void Room::Enter(shared_ptr<Player> _player)
{
	WRITE_LOCK;
	m_players[_player->GetPlayerId()] = _player;
}

void Room::Leave(shared_ptr<Player> _player)
{
	WRITE_LOCK;
	m_players.erase(_player->GetPlayerId());
}

void Room::Broadcast(shared_ptr<Sender> _sender)
{
	WRITE_LOCK;
	for (auto& p : m_players)
	{
		p.second->GetOwnerSession()->Send(_sender);
	}
}