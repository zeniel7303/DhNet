#include "stdafx.h"
#include "Player.h"
#include "GameSession.h"

Player::Player(uint64 _id, string _name, shared_ptr<GameSession> _session)
{
	m_playerId = _id;
	m_name = _name;
	m_ownerSession = _session;
}

Player::~Player()
{
}
