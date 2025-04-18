#pragma once
#include "RoomController.h"
#include "GameSession.h"
#include "Player.h"
#include "GameServer.h"

bool HandleReqRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqRoomEnter = reinterpret_cast<ReqRoomEnter*>(_header);
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = gameSession->GetPlayer();
	GameServer::Instance().GetSystem<RoomSystem>()->GetRoom()->DoAsync(&Room::Enter, player);

	auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomEnter>();
	senderAndPacket.first->Init(player->GetPlayerId(), player->GetPlayerName());
	GameServer::Instance().GetSystem<RoomSystem>()->GetRoom()->DoAsync(&Room::Broadcast, senderAndPacket.second);

	return true;
}

bool HandleReqRoomChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqRoomChat = reinterpret_cast<ReqRoomChat*>(_header);
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = gameSession->GetPlayer();

	auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomChat>();
	senderAndPacket.first->Init(player->GetPlayerId(), player->GetPlayerName(), reqRoomChat->m_message);
	GameServer::Instance().GetSystem<RoomSystem>()->GetRoom()->DoAsync(&Room::Broadcast, senderAndPacket.second);

	return true;
}

bool HandleReqRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqRoomExit = reinterpret_cast<ReqRoomExit*>(_header);
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = gameSession->GetPlayer();
	GameServer::Instance().GetSystem<RoomSystem>()->GetRoom()->DoAsync(&Room::Leave, player);

	auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomExit>();
	senderAndPacket.first->Init(player->GetPlayerId(), player->GetPlayerName());
	GameServer::Instance().GetSystem<RoomSystem>()->GetRoom()->DoAsync(&Room::Broadcast, senderAndPacket.second);

	return true;
}