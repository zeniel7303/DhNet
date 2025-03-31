#pragma once
#include "stdafx.h"
#include "../DhNet_Protocol/PacketList.h"
#include "../ServerCore/PacketHandler.h"
#include "Player.h"

bool HandleReqLoginPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto reqLogin = reinterpret_cast<ReqLogin*>(_header);
	auto gameSession = static_pointer_cast<GameSession>(_session);

	auto num = idGenerator++;
	auto player = make_shared<Player>(num, "TempUser" + std::to_string(num), gameSession);

	auto sender = Sender::Alloc<ResLogin>();
	auto packet = sender->GetWritePointer<ResLogin>();
	packet->Init(PacketEnum::Res_Login, sizeof(ResLogin));
	packet->m_isSuccess = true;

	_session->Send(sender);

	return true;
}