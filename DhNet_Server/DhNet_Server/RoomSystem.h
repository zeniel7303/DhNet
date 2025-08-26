#pragma once
#include "Room.h"

class RoomSystem
{
private:
	USE_LOCK
	std::map<int, std::shared_ptr<Room>> m_rooms;

public:
	RoomSystem();
	~RoomSystem();

	std::shared_ptr<Room> MakeRoom();
	std::shared_ptr<Room> GetRoom(int32 roomIndex = 0);
	std::map<int, std::shared_ptr<Room>> GetRooms();
};