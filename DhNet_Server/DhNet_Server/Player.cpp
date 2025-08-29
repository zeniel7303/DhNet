#include "stdafx.h"
#include "Player.h"
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
    /* shared_from_this()�� �ܼ��� enable_shared_from_this<T>�� ����� _wptr�� shared_ptr�� ĳ�����Ͽ� ��ȯ�Ѵ�.
       �׷��� enable_shared_from_this<T>�� ��� _Wptr�� enable_shared_from_this<T>�� �����ڿ��� ���õǴ� ���� �ƴ϶�, 
       std::make_shared()���� ���õȴ�.std::make_shared<MatchMother>()�� ���ο���, _Wptr ������ MatchMother �����ں��� �ڿ��� �Ͼ��.
       ���� ������ ���ο��� shared_from_this()�� ȣ���ϴ� ����, ���� ���õ��� ���� _Wptr�� ȣ���ϴ� �����̹Ƿ� ���α׷��� �״´�.*/
    // GameServer::Instance().GetSystem<PlayerSystem>()->Add(std::static_pointer_cast<Player>(shared_from_this()));
}

Player::~Player()
{
	// std::cout << "~Player" << std::endl;
}

void Player::DisConnect()
{
	if (m_currentRoom.expired())
	{
		GameServer::Instance().GetSystem<LobbySystem>()->LeaveLobby(shared_from_this());
	}
	else
	{
		auto room = m_currentRoom.lock();
		room->DoAsync(room, &Room::Leave, shared_from_this(), true);
	}

	GameServer::Instance().GetSystem<PlayerSystem>()->Remove(shared_from_this());
}