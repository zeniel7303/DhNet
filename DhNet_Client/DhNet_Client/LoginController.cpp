#pragma once
#include "LoginController.h"

bool HandleResLoginPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto resLogin = reinterpret_cast<ResLogin*>(_header);
	auto serverSession = std::static_pointer_cast<ServerSession>(_session);
	serverSession->m_playerId = resLogin->m_playerId;
	serverSession->m_playerName = resLogin->m_playerName;

	auto senderAndPacket1 = Sender::GetSenderAndPacket<ReqLobbyChat>();
	senderAndPacket1.first->Init(PacketEnum::Req_LobbyChat, sizeof(ReqLobbyChat));
	strcpy_s(senderAndPacket1.first->m_message, "Login Success");
	serverSession->Send(senderAndPacket1.second);

	/*auto senderAndPacket2 = Sender::GetSenderAndPacket<ReqRoomEnter>();
	senderAndPacket2.first->Init();
	serverSession->Send(senderAndPacket2.second);*/

	return true;
}