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

	auto room = GameServer::Instance().GetSystem<LobbySystem>()->GetNotFullRoom();
	if (!room)
	{
		room = GameServer::Instance().GetSystem<LobbySystem>()->MakeRoom();
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

	if (player == nullptr)
	{
		std::cout << "What?" << std::endl;
		return true;
	}

	if (const auto room = player->GetCurrentRoom().lock())
	{
		room->DoAsync([room, pid = player->GetPlayerId(), msg = std::string(reqRoomChat->m_message)] {
			room->HandleChat(pid, msg);
		});
	}

	return true;
}

bool HandleReqRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqRoomExit = reinterpret_cast<ReqRoomExit*>(_header);
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = gameSession->GetPlayer();
	
	if (const auto room = player->GetCurrentRoom().lock())
	{
		room->DoAsync(room, &Room::Leave, player, false);
	}

	return true;
}