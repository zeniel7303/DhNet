#pragma once
#include "stdafx.h"
#include "../ServerCore/Session.h"
#include "../DhNet_Protocol/PacketList.h"

bool HandleReqLobbyChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
