#pragma once
#include "../DhNet_Protocol/PacketList.h"
#include "../ServerCore/PacketHandler.h"

bool HandleTestPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);