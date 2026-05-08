#include "stdafx.h"
#include "LobbyController.h"
#include "GameSession.h"
#include "Player.h"
#include "Lobby.h"
#include "GameServer.h"

bool HandleReqLobbyChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqLobbyChat = reinterpret_cast<ReqLobbyChat*>(_header);
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = gameSession->GetPlayer();
	if (!player) return false;

	player->LobbyChat(std::string(reqLobbyChat->m_message));

	return true;
}

bool HandleReqRoomListPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto gameSession = std::static_pointer_cast<GameSession>(_session);
	auto player = gameSession->GetPlayer();
	if (!player) return false;

	auto rooms = GameServer::Instance().GetSystem<RoomSystem>()->GetRooms();

	auto senderAndPacket = Sender::GetSenderAndPacket<ResRoomList>();
	senderAndPacket.first->Init();
	for (auto& [index, room] : rooms)
	{
		senderAndPacket.first->AddRoom(room->GetRoomIndex(), room->GetPlayerCount(), MAX_ROOM_PLAYER);
	}

	player->GetOwnerSession()->Send(senderAndPacket.second);

	return true;
}
