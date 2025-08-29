#pragma once
#include "LobbyController.h"
#include "GameSession.h"
#include "Player.h"
#include "GameServer.h"

bool HandleReqLobbyChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqLobbyChat = reinterpret_cast<ReqLobbyChat*>(_header);
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = gameSession->GetPlayer();

	if (!player)
	{
		return true;
	}
	
	auto lobby = GameServer::Instance().GetSystem<LobbySystem>()->GetLobby();
	if (lobby)
	{
		auto senderAndPacket = Sender::GetSenderAndPacket<NotiLobbyChat>();
		senderAndPacket.first->Init(player->GetPlayerId(), player->GetPlayerName(), reqLobbyChat->m_message);
		lobby->DoAsync(lobby, &Lobby::Broadcast, senderAndPacket.second);
	}

	return true;
}
