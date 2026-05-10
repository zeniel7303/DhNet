#include "RoomController.h"

std::atomic<bool> g_inRoom{false};

bool HandleResRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto pkt = reinterpret_cast<ResRoomEnter*>(_header);
	if (!pkt->m_isSuccess)
	{
		std::cout << "[Room] Enter failed\n";
		g_inRoom = false;
	}
	return true;
}

bool HandleNotiRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomEnter = reinterpret_cast<NotiRoomEnter*>(_header);
	std::cout << notiRoomEnter->m_playerName << " Enter " << std::endl;
	g_inRoom = true;
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
	std::cout << notiRoomExit->m_playerName << " Exit " << std::endl;

	auto serverSession = std::static_pointer_cast<ServerSession>(_session);
	if (notiRoomExit->m_playerId == serverSession->m_playerId)
		g_inRoom = false;

	return true;
}
