#include "stdafx.h"
#include "Room.h"

void Room::Enter(std::shared_ptr<Player> _player)
{
	if (static_cast<int32>(m_players.size()) >= MAX_ROOM_PLAYER)
	{
		// �� ���� ������ ���� TryReserveSlot�� ������ ����ȴٴ� ���� �Ͽ� ReleaseReservedSlot ȣ��
		ReleaseReservedSlot();
		_player->EnterRoomFailed();
		return;
	}

	auto [it, result] = m_players.try_emplace(_player->GetPlayerId(), _player);
	if (!result)
	{
		// �� ���� ������ ���� TryReserveSlot�� ������ ����ȴٴ� ���� �Ͽ� ReleaseReservedSlot ȣ��
		ReleaseReservedSlot();
		_player->EnterRoomFailed();
		return;
	}

	const auto self = std::static_pointer_cast<Room>(shared_from_this());
	_player->SetCurrentRoom(self);

	if (!_player->GetOwnerSession())
	{
		// Enter 실행 전 연결이 끊긴 경우: 맵·슬롯 정리 (NotiRoomEnter는 아직 미전송이므로 보상 불필요)
		m_players.erase(_player->GetPlayerId());
		ReleaseReservedSlot();
		return;
	}

	_player->EnterRoom();

	std::cout << m_players.size() << std::endl;
}

void Room::Leave(std::shared_ptr<Player> _player)
{
	auto it = m_players.find(_player->GetPlayerId());
	if (it != m_players.end())
	{
		m_players.erase(it);
		m_availableSlots.fetch_add(1);
	}

	std::cout << m_players.size() << std::endl;
}

void Room::Broadcast(std::shared_ptr<Sender> _sender)
{
	for (auto& p : m_players)
	{
		if (auto session = p.second->GetOwnerSession())
			session->Send(_sender);
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