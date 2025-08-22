#include "stdafx.h"
#include "RoomSystem.h"
#include "../../DhUtil/ObjectPool.h"

RoomSystem::RoomSystem()
{
}

RoomSystem::~RoomSystem()
{
}

std::shared_ptr<Room> RoomSystem::MakeRoom()
{
	// Temp (������ �� 1����)
	WRITE_LOCK
	auto room = ObjectPool<Room>::MakeShared();
	room->SetRoomIndex(0);
	m_rooms[0] = room;
	return room;
}

std::shared_ptr<Room> RoomSystem::GetRoom(int32 roomIndex)
{
	{
		READ_LOCK
		auto it = m_rooms.find(roomIndex);
		if (it != m_rooms.end())
		{
			return it->second;
		}
	}

	return MakeRoom();
}