#pragma once
#include "RoomController.h"

#include "ServerSession.h"

bool HandleResRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto resRoomEnter = reinterpret_cast<ResRoomEnter*>(_header);
	auto serverSession = std::static_pointer_cast<ServerSession>(_session);

	if (resRoomEnter->m_isSuccess)
	{
		std::cout << "Room Enter Success" << std::endl;
	}
	else
	{
		std::cout << "Room Enter Failed" << std::endl;
	}
	
	return true;
}

bool HandleNotiRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomEnter = reinterpret_cast<NotiRoomEnter*>(_header);
	
	std::cout << notiRoomEnter->m_playerName << " Enter " << std::endl;

	return true;
}

bool HandleNotiRoomChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomChat = reinterpret_cast<NotiRoomChat*>(_header);

	std::cout << notiRoomChat->m_playerName << " : " << notiRoomChat->m_message << std::endl;

	return true;
}

bool HandleResRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto resRoomExit = reinterpret_cast<ResRoomExit*>(_header);

	if (resRoomExit->m_isSuccess)
	{
		std::cout << "Room Exit Success" << std::endl;
	}
	else
	{
		std::cout << "Room Exit Failed" << std::endl;
	}
	
	return true;
}

bool HandleNotiRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomExit = reinterpret_cast<NotiRoomExit*>(_header);

	std::cout << notiRoomExit->m_playerName << " Exit " << std::endl;

	return true;
}
