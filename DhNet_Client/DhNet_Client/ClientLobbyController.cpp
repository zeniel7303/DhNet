#include "ClientLobbyController.h"

bool HandleResLobbyEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto pkt = reinterpret_cast<ResLobbyEnter*>(_header);
	LOG_INFO("[Lobby] Entered lobby index={}", pkt->m_lobbyIndex);

	auto senderAndPacket = Sender::GetSenderAndPacket<ReqRoomEnter>();
	senderAndPacket.first->Init();
	_session->Send(senderAndPacket.second);

	return true;
}

bool HandleNotiLobbyPlayerEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto pkt = reinterpret_cast<NotiLobbyPlayerEnter*>(_header);
	LOG_INFO("[Lobby] Player entered: {}", pkt->m_playerName);
	return true;
}

bool HandleNotiLobbyPlayerExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto pkt = reinterpret_cast<NotiLobbyPlayerExit*>(_header);
	LOG_INFO("[Lobby] Player exited: {}", pkt->m_playerName);
	return true;
}

bool HandleNotiLobbyChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto pkt = reinterpret_cast<NotiLobbyChat*>(_header);
	LOG_INFO("[Lobby] Chat: {}: {}", pkt->m_playerName, pkt->m_message);
	return true;
}
