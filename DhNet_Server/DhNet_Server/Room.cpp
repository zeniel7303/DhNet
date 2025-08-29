#include "stdafx.h"
#include "Room.h"

void Room::Enter(std::shared_ptr<Player> _player)
{
	if (static_cast<int32>(m_players.size()) >= MAX_ROOM_PLAYER)
	{
		// 이 곳에 들어오기 전에 TryReserveSlot는 무조건 실행된다는 가정 하에 ReleaseReservedSlot 호출
		ReleaseReservedSlot();
		_player->EnterRoomFailed();
		return;
	}

	auto [it, result] = m_players.try_emplace(_player->GetPlayerId(), _player);
	if (!result)
	{
		// 이 곳에 들어오기 전에 TryReserveSlot는 무조건 실행된다는 가정 하에 ReleaseReservedSlot 호출
		ReleaseReservedSlot();
		_player->EnterRoomFailed();
		return;
	}

	const auto self = std::static_pointer_cast<Room>(shared_from_this());
	_player->SetCurrentRoom(self);
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