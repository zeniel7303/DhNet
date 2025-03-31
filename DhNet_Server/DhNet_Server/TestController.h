#pragma once
#include "../DhNet_Protocol/PacketList.h"
#include "../ServerCore/PacketHandler.h"
#include "GameSessionManager.h"

bool HandleTestPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto temp = reinterpret_cast<TestPacket*>(_header);

	cout << "OnRecv Len " << temp->m_dataSize << " / " << temp->m_test << endl;

	auto sender = Sender::Alloc<TestPacket>();
	auto packet = sender->GetWritePointer<TestPacket>();
	packet->Init(PacketEnum::Test, sizeof(TestPacket));
	strcpy_s(packet->m_test, "Test(Server)");

	GSessionManager.Broadcast(sender);

	return true;
}