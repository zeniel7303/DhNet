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

	auto room = GameServer::Instance().GetSystem<RoomSystem>()->GetNotFullRoom();
	if (!room)
	{
		room = GameServer::Instance().GetSystem<RoomSystem>()->MakeRoom();
		ASSERT_CRASH(room != nullptr)
	}
	
	room->DoAsync(room, &Room::Enter, player);

	return true;
}

bool HandleReqRoomChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqRoomChat = reinterpret_cast<ReqRoomChat*>(_header);
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = gameSession->GetPlayer();

	player->RoomChat(reqRoomChat->m_message);

	return true;
}

bool HandleReqRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqRoomExit = reinterpret_cast<ReqRoomExit*>(_header);
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = gameSession->GetPlayer();
	
	player->LeaveRoom();

	return true;
}