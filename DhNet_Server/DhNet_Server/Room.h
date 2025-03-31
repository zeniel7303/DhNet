#pragma once
#include "GameSession.h"
#include "Player.h"

class Room
{
private:
	USE_LOCK;
	map<uint64, shared_ptr<Player>> m_players;

public:
	void Enter(shared_ptr<Player> _player);
	void Leave(shared_ptr<Player> _player);
	void Broadcast(shared_ptr<Sender> _sender);
};

extern Room GRoom;