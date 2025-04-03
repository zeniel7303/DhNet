#pragma once
#include "LoginController.h"

bool HandleResLoginPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto resLogin = reinterpret_cast<ResLogin*>(_header);
	auto serverSession = std::static_pointer_cast<ServerSession>(_session);
	serverSession->m_playerId = resLogin->m_playerId;
	serverSession->m_playerName = resLogin->m_playerName;

	auto senderAndPacket = Sender::GetSenderAndPacket<ReqRoomEnter>();
	senderAndPacket.first->Init();
	_session->Send(senderAndPacket.second);

	return true;
}