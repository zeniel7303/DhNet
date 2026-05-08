#pragma once
#include "../DhNet_Protocol/PacketList.h"
#include "../ServerCore/PacketHandler.h"

bool HandleReqLobbyChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
bool HandleReqRoomListPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
