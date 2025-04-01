#pragma once
#include "RoomController.h"

bool HandleNotiRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomEnter = reinterpret_cast<NotiRoomEnter*>(_header);

	cout << notiRoomEnter->m_playerName << " ÀÔÀå " << endl;

	return true;
}

bool HandleNotiRoomChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomChat = reinterpret_cast<NotiRoomChat*>(_header);

	cout << notiRoomChat->m_playerName << " : " << notiRoomChat->m_message << endl;

	return true;
}

bool HandleNotiRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomExit = reinterpret_cast<NotiRoomExit*>(_header);

	cout << notiRoomExit->m_playerName << " ÅðÀå " << endl;

	return true;
}