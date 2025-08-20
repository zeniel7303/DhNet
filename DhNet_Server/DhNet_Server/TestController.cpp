#pragma once
#include "stdafx.h"
#include "TestController.h"
#include "../ServerCore/Sender.h"

bool HandleTestPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto temp = reinterpret_cast<TestPacket*>(_header);

	// std::cout << "OnRecv Len " << temp->m_dataSize << " / " << temp->m_test << std::endl;

	auto sender = Sender::Alloc<TestPacket>();
	auto packet = sender->GetWritePointer<TestPacket>();
	packet->Init(PacketEnum::Test, sizeof(TestPacket));
	strcpy_s(packet->m_test, "Test(Server)");

	return true;
}