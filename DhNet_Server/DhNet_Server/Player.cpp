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

    // [ shared_from_this()�� ������ �ȿ��� ȣ���ϸ� �ȵȴ�. ]
    /* shared_from_this()�� �ܼ��� enable_shared_from_this<T>�� ����� _Wptr�� shared_ptr�� ĳ�����Ͽ� ��ȯ�Ѵ�.
       �׷��� enable_shared_from_this<T>�� ��� _Wptr�� enable_shared_from_this<T>�� �����ڿ��� ���õǴ� ���� �ƴ϶�, 
       std::make_shared()���� ���õȴ�.std::make_shared<MatchMother>()�� ���ο���, _Wptr ������ MatchMother �����ں��� �ڿ��� �Ͼ��.
       ���� ������ ���ο��� shared_from_this()�� ȣ���ϴ� ����, ���� ���õ��� ���� _Wptr�� ȣ���ϴ� �����̹Ƿ� ���α׷��� �״´�.*/
    // GameServer::Instance().GetSystem<PlayerSystem>()->Add(std::static_pointer_cast<Player>(shared_from_this()));
}

Player::~Player()
{
    std::cout << "~Player" << std::endl;
}

void Player::LeaveRoom()
{
    // std::cout << m_name << "����" << std::endl;

    GameServer::Instance().GetSystem<RoomSystem>()->GetRoom()->DoAsync(&Room::Leave, shared_from_this());

    auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomExit>();
    senderAndPacket.first->Init(m_playerId, m_name);
    GameServer::Instance().GetSystem<RoomSystem>()->GetRoom()->DoAsync(&Room::Broadcast, senderAndPacket.second);

    // ������ �� �����ٴ°� �� ���ſ� ����.
    GameServer::Instance().GetSystem<PlayerSystem>()->Remove(std::static_pointer_cast<Player>(shared_from_this()));
}