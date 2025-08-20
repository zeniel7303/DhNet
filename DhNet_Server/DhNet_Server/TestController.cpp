#pragma once
#include "stdafx.h"
#include "TestController.h"
#include "GameSession.h"

bool HandleTestPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto testPacket = reinterpret_cast<TestPacket*>(_header);

	// std::cout << "OnRecv Len " << testPacket->m_dataSize << " / " << testPacket->m_test << std::endl;

	auto senderAndPacket = Sender::GetSenderAndPacket<TestPacket>();
	senderAndPacket.first->Init(PacketEnum::Test, sizeof(TestPacket));
	strcpy_s(senderAndPacket.first->m_test, "Test(Server)");
	_session->Send(senderAndPacket.second);

	return true;
}