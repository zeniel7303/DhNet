#pragma once
#include "../../DhNet_Server/DhNet_Protocol/PacketList.h"
#include "../../DhNet_Server/ServerCore/PacketHandler.h"

bool HandleNotiRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
bool HandleNotiRoomChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
bool HandleNotiRoomExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);