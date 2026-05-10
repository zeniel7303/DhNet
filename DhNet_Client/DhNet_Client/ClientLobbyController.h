#pragma once
#include "../../DhNet_Server/DhNet_Protocol/PacketList.h"
#include "../../DhNet_Server/ServerCore/PacketHandler.h"
#include "ServerSession.h"

bool HandleResLobbyEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
bool HandleNotiLobbyPlayerEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
bool HandleNotiLobbyPlayerExitPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
bool HandleNotiLobbyChatPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);
