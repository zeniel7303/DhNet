#pragma once
#include "stdafx.h"
#include "LoginController.h"
#include "GameSession.h"
#include "Player.h"
#include "GameServer.h"

bool HandleReqLoginPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqLogin = reinterpret_cast<ReqLogin*>(_header);
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = std::make_shared<Player>(gameSession);

	gameSession->SetPlayer(player);
	GameServer::Instance().GetSystem<PlayerSystem>()->Add(player);

	auto senderAndPacket = Sender::GetSenderAndPacket<ResLogin>();
	senderAndPacket.first->Init(player->GetPlayerId(), player->GetPlayerName());
	_session->Send(senderAndPacket.second);

	return true;
}