#pragma once
#include "../DhNet_Protocol/PacketList.h"
#include "../ServerCore/PacketHandler.h"
#include "GameSessionManager.h"
#include "Room.h"

bool HandleReqRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqRoomEnter = reinterpret_cast<ReqRoomEnter*>(_header);
	auto gameSession = static_pointer_cast<GameSession>(_session);

	auto player = gameSession->GetPlayer();
	GRoom.Enter(player);

	auto sender = Sender::Alloc<ResRoomEnter>();
	auto packet = sender->GetWritePointer<ResRoomEnter>();
	packet->Init(PacketEnum::Res_RoomEnter, sizeof(ResRoomEnter));
	packet->m_isSuccess = true;

	GSessionManager.Broadcast(sender);

	return true;
}