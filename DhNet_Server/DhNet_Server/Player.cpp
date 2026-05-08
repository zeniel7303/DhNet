#include "stdafx.h"
#include "Player.h"
#include "../DhNet_Protocol/PacketList.h"
#include "GameServer.h"
#include "GameSession.h"
#include "Room.h"
#include "Lobby.h"
#include "LobbySystem.h"

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
	if (const auto room = m_currentRoom.lock())
	{
		room->DoAsync(room, &Room::Leave, shared_from_this());

		auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomExit>();
		senderAndPacket.first->Init(m_playerId, m_name);
		room->DoAsync(room, &Room::Broadcast, senderAndPacket.second);
	}

	m_currentRoom.reset();
}

void Player::LeaveRoomAndReenterLobby()
{
	LeaveRoom();

	auto lobby = GameServer::Instance().GetSystem<LobbySystem>()->AssignLobby();
	if (lobby)
		lobby->DoAsync(lobby, &Lobby::Enter, shared_from_this());
}

void Player::LeaveRoomFailed()
{
	/*auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomExit>();
	senderAndPacket.first->Init(false);
	GetOwnerSession()->Send(senderAndPacket.second);*/
}

void Player::RoomChat(std::string _message)
{
	if (const auto room = m_currentRoom.lock())
	{
		auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomChat>();
		senderAndPacket.first->Init(m_playerId, m_name, _message.c_str());
		room->DoAsync(room, &Room::Broadcast, senderAndPacket.second);
	}
}


void Player::LeaveLobby()
{
	if (const auto lobby = m_currentLobby.lock())
	{
		lobby->DoAsync(lobby, &Lobby::Exit, shared_from_this());
	}
	m_currentLobby.reset();
}

void Player::LobbyChat(std::string _message)
{
	if (const auto lobby = m_currentLobby.lock())
	{
		lobby->DoAsync(lobby, &Lobby::Chat, shared_from_this(), _message);
	}
}
