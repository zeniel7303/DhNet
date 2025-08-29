#pragma once
#include "LobbyController.h"
#include <iostream>

bool HandleNotiLobbyChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto serverSession = std::static_pointer_cast<ServerSession>(_session);
	auto notiLobbyChat = reinterpret_cast<NotiLobbyChat*>(_header);
	// std::cout << "Lobby - " << notiLobbyChat->m_playerName << " : " << notiLobbyChat->m_message << std::endl;
	
	return true;
}