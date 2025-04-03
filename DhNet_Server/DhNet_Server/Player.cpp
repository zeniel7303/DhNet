#include "stdafx.h"
#include "Player.h"
#include "../DhNet_Protocol/PacketList.h"
#include "GameSession.h"
#include "Room.h"

Player::Player(uint64 _id, std::string _name, std::shared_ptr<GameSession> _session)
{
	m_playerId = _id;
	m_name = _name;
	m_ownerSession = _session;
}

Player::~Player()
{
    std::cout << "~Player" << std::endl;
}

void Player::LeaveRoom()
{
    std::cout << m_name << "³ª°¨" << std::endl;

    GRoom.PushJob(&Room::Leave, shared_from_this());

    auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomExit>();
    senderAndPacket.first->Init(m_playerId, m_name);
    GRoom.PushJob(&Room::Broadcast, senderAndPacket.second);
}