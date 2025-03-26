#include "stdafx.h"
#include "ServerSession.h"

void ServerSession::OnConnected()
{
	cout << "Connected To Server" << endl;

	auto sender = Sender::Alloc<TestPacket>();
	auto packet = sender->GetWritePointer<TestPacket>();
	packet->Init(PacketEnum::Test, sizeof(TestPacket));
	strcpy_s(packet->m_test, "Connect");
	Send(sender);
}

bool ServerSession::OnRecv(PacketHeader* _packet)
{
	return PacketHandler::Instance().Process(_packet->m_packetNum, _packet, static_pointer_cast<Session>(shared_from_this()));
}

void ServerSession::OnSend(int32 _len)
{
	// cout << "OnSend Len = " << _len << endl;
}

void ServerSession::OnDisconnected()
{
	cout << "Disconnected" << endl;
}