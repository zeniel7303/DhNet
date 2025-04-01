#pragma once
#include "../DhNet_Protocol/PacketList.h"
#include "../ServerCore/PacketHandler.h"

bool HandleReqRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
bool HandleReqRoomChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
bool HandleReqRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);