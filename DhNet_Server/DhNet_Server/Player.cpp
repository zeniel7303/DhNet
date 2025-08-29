#include "stdafx.h"
#include "Player.h"
#include "../DhNet_Protocol/PacketList.h"
#include "GameServer.h"
#include "GameSession.h"
#include "Room.h"

Player::Player(std::shared_ptr<GameSession> _session)
{
	auto num = GameServer::Instance().GetSystem<UniqueIdGenerationSystem>()->GenerateUniqueId();

	m_playerId = num;
	m_name = "TempUser" + std::to_string(num);
	m_ownerSession = _session;

    // [ shared_from_this()는 생성자 안에서 호출하면 안된다. ]
    /* shared_from_this()는 단순히 enable_shared_from_this<T>의 멤버인 _wptr을 shared_ptr로 캐스팅하여 반환한다.
       그런데 enable_shared_from_this<T>의 멤버 _Wptr은 enable_shared_from_this<T>의 생성자에서 셋팅되는 것이 아니라, 
       std::make_shared()에서 셋팅된다.std::make_shared<MatchMother>()의 내부에서, _Wptr 셋팅이 MatchMother 생성자보다 뒤에서 일어난다.
       따라서 생성자 내부에서 shared_from_this()를 호출하는 것은, 아직 셋팅되지 않은 _Wptr을 호출하는 행위이므로 프로그램이 죽는다.*/
    // GameServer::Instance().GetSystem<PlayerSystem>()->Add(std::static_pointer_cast<Player>(shared_from_this()));
}

Player::~Player()
{
    // std::cout << "~Player" << std::endl;
}

void Player::EnterRoom()
{
	/*auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomEnter>();
	senderAndPacket.first->Init(true);
	GetOwnerSession()->Send(senderAndPacket.second);*/
	
	if (const auto room = m_currentRoom.lock())
	{
		auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomEnter>();
		senderAndPacket.first->Init(m_playerId, m_name);
		room->DoAsync(room, &Room::Broadcast, senderAndPacket.second);
	}
}

void Player::EnterRoomFailed()
{
	/*auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomEnter>();
	senderAndPacket.first->Init(false);
	GetOwnerSession()->Send(senderAndPacket.second);*/
}

void Player::LeaveRoom()
{
    // std::cout << m_name << "나감" << std::endl;

	if (const auto room = m_currentRoom.lock())
	{
		room->DoAsync(room, &Room::Leave, shared_from_this());

		auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomExit>();
		senderAndPacket.first->Init(m_playerId, m_name);
		room->DoAsync(room, &Room::Broadcast, senderAndPacket.second);
	}

	// 방 정보 해제 (약한 참조 리셋)
	m_currentRoom.reset();

	/*auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomExit>();
	senderAndPacket.first->Init(true);
	GetOwnerSession()->Send(senderAndPacket.second);*/
	
    // 지금은 방 나갔다는건 겜 끈거와 같다.
    GameServer::Instance().GetSystem<PlayerSystem>()->Remove(std::static_pointer_cast<Player>(shared_from_this()));
}

void Player::LeaveRoomFailed()
{
	/*auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomExit>();
	senderAndPacket.first->Init(false);
	GetOwnerSession()->Send(senderAndPacket.second);*/
}

void Player::RoomChat(std::string _message)
{
	if (m_currentRoom.expired()) return;
	
	if (const auto room = m_currentRoom.lock())
	{
		auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomChat>();
		senderAndPacket.first->Init(m_playerId, m_name, _message.c_str());
		room->DoAsync(room, &Room::Broadcast, senderAndPacket.second);
	}
}
