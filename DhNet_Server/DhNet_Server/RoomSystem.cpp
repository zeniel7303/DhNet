#include "stdafx.h"
#include "RoomSystem.h"
#include "../../DhUtil/ObjectPool.h"

RoomSystem::RoomSystem()
{
	m_roomNum = 0;
}

RoomSystem::~RoomSystem()
{
}

std::shared_ptr<Room> RoomSystem::MakeRoom()
{
	WRITE_LOCK
	
	auto room = ObjectPool<Room>::MakeShared();
	room->SetRoomIndex(m_roomNum);
	m_rooms[m_roomNum] = room;
	m_roomNum.fetch_add(1);
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

	return nullptr;
}

std::map<int, std::shared_ptr<Room>> RoomSystem::GetRooms()
{
	READ_LOCK
	return m_rooms; // return a copy for safe iteration outside lock
}

std::shared_ptr<Room> RoomSystem::GetEmptyRoom()
{
	READ_LOCK
	for (auto& [index, room] : m_rooms)
	{
		if (room && room->GetPlayerCount() < MAX_ROOM_PLAYER)
			return room;
	}
	return nullptr;
}