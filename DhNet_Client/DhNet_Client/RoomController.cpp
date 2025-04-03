#pragma once
#include "RoomController.h"

bool HandleNotiRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomEnter = reinterpret_cast<NotiRoomEnter*>(_header);

	std::cout << notiRoomEnter->m_playerName << " ÀÔÀå " << std::endl;

	return true;
}

bool HandleNotiRoomChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomChat = reinterpret_cast<NotiRoomChat*>(_header);

	std::cout << notiRoomChat->m_playerName << " : " << notiRoomChat->m_message << std::endl;

	return true;
}

bool HandleNotiRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomExit = reinterpret_cast<NotiRoomExit*>(_header);

	std::cout << notiRoomExit->m_playerName << " ÅðÀå " << std::endl;

	return true;
}