#include "stdafx.h"
#include "Room.h"
#include "Player.h"
#include "GameServer.h"
#include "GameSession.h"
#include "../DhNet_Protocol/PacketList.h"

void Room::Enter(std::shared_ptr<Player> _player)
{
	// [Room authoritative] �� �Լ��� �ݵ�� Room�� DoAsync ���ؽ�Ʈ������ ȣ��Ǿ�� ��
	if (static_cast<int32>(m_players.size()) >= MAX_ROOM_PLAYER)
	{
		// ���� ���� ���� ��� ����
		ReleaseReservedSlot();

		// ���� ���� ����
		auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomEnter>();
		senderAndPacket.first->Init(false);
		_player->GetOwnerSession()->Send(senderAndPacket.second);
		return;
	}

	auto fut = GameServer::Instance().GetSystem<LobbySystem>()->LeaveLobbyAsync(_player);
	fut.get();

	auto [it, inserted] = m_players.try_emplace(_player->GetPlayerId(), _player);
	if (!inserted)
	{
		// �ߺ� ����: ���� ���� �� ���� ����
		ReleaseReservedSlot();

		auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomEnter>();
		senderAndPacket.first->Init(false);
		_player->GetOwnerSession()->Send(senderAndPacket.second);
		return;
	}

	// �÷��̾� �� ���� (���� ����)
	const auto self = std::static_pointer_cast<Room>(shared_from_this());
	_player->SetCurrentRoom(self);

	// ���� ���� ���� (����)
	{
		auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomEnter>();
		senderAndPacket.first->Init(true);
		_player->GetOwnerSession()->Send(senderAndPacket.second);
	}

	// ���� �˸� ��ε�ĳ��Ʈ (��ü)
	{
		auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomEnter>();
		senderAndPacket.first->Init(_player->GetPlayerId(), _player->GetPlayerName());
		Broadcast(senderAndPacket.second);
	}
}

void Room::Leave(std::shared_ptr<Player> _player, bool _isDisconnect)
{
	// [Room authoritative] �� �Լ��� �ݵ�� Room�� DoAsync ���ؽ�Ʈ������ ȣ��Ǿ�� ��
	auto it = m_players.find(_player->GetPlayerId());
	if (it == m_players.end())
	{
		// �̹� ���� �÷��̾�: ����
		return;
	}

	const uint64 pid = _player->GetPlayerId();
	const std::string name = _player->GetPlayerName();

	m_players.erase(it);
	m_availableSlots.fetch_add(1);

	// �÷��̾��� ���� �� ���� ����
	_player->SetCurrentRoom(nullptr);

	// ���� �˸� ��ε�ĳ��Ʈ (��ü)
	{
		auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomExit>();
		senderAndPacket.first->Init(pid, name);
		Broadcast(senderAndPacket.second);
	}

	// ���� ���� ���� (����)
	{
		auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomExit>();
		senderAndPacket.first->Init(true);
		_player->GetOwnerSession()->Send(senderAndPacket.second);
	}

	if (_isDisconnect)
	{
		return;
	}

	auto fut = GameServer::Instance().GetSystem<LobbySystem>()->EnterLobbyAsync(_player);
	fut.get();
}

void Room::HandleChat(uint64 _playerId, const std::string& _message)
{
	// [Room authoritative] �� �Լ��� �ݵ�� Room�� DoAsync ���ؽ�Ʈ������ ȣ��Ǿ�� ��
	auto it = m_players.find(_playerId);
	if (it == m_players.end())
	{
		// TODO : �α�
		return;
	}

	// ä�� ��ε�ĳ��Ʈ
	auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomChat>();
	senderAndPacket.first->Init(_playerId, it->second->GetPlayerName(), _message.c_str());
	Broadcast(senderAndPacket.second);
}

void Room::AdminBroadcast(const std::string& _message)
{
	auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomChat>();
	senderAndPacket.first->Init(0, "ADMIN", _message.c_str());
	Broadcast(senderAndPacket.second);
}

void Room::Broadcast(std::shared_ptr<Sender> _sender)
{
	for (auto& p : m_players)
	{
		p.second->GetOwnerSession()->Send(_sender);
	}
}

bool Room::TryReserveSlot()
{
	while (true)
	{
		int32 avail = m_availableSlots.load();
		if (avail <= 0)
		{
			return false;
		}

		// lock-free CAS loop
		if (m_availableSlots.compare_exchange_weak(avail, avail - 1))
		{
			return true;
		}
	}
}

void Room::ReleaseReservedSlot()
{
	m_availableSlots.fetch_add(1);
}