#include "RoomController.h"

std::atomic<bool> g_inRoom{false};

bool HandleResRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto pkt = reinterpret_cast<ResRoomEnter*>(_header);
	if (!pkt->m_isSuccess)
	{
		LOG_WARN("[Room] Enter failed");
		g_inRoom = false;
	}
	return true;
}

bool HandleNotiRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomEnter = reinterpret_cast<NotiRoomEnter*>(_header);
	LOG_INFO("{} Enter", notiRoomEnter->m_playerName);
	g_inRoom = true;
	return true;
}

bool HandleNotiRoomChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomChat = reinterpret_cast<NotiRoomChat*>(_header);
	LOG_INFO("{} : {}", notiRoomChat->m_playerName, notiRoomChat->m_message);
	return true;
}

bool HandleNotiRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto notiRoomExit = reinterpret_cast<NotiRoomExit*>(_header);
	LOG_INFO("{} Exit", notiRoomExit->m_playerName);

	auto serverSession = std::static_pointer_cast<ServerSession>(_session);
	if (notiRoomExit->m_playerId == serverSession->GetPlayerId())
		g_inRoom = false;

	return true;
}
