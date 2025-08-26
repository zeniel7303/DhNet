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
	// Temp (무조건 방 1개만)
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
		if (const auto it = m_rooms.find(roomIndex); it != m_rooms.end())
		{
			return it->second;
		}
	}

	return MakeRoom();
}

std::map<int, std::shared_ptr<Room>> RoomSystem::GetRooms()
{
	READ_LOCK;
	return m_rooms; // return a copy for safe iteration outside lock
}