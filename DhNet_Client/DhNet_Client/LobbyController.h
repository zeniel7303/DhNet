#pragma once
#include "../../DhNet_Server/DhNet_Protocol/PacketList.h"
#include "../../DhNet_Server/ServerCore/PacketHandler.h"
#include "ServerSession.h"

bool HandleNotiLobbyChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
