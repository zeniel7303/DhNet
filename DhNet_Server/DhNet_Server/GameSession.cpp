#include "stdafx.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "TempPacketList.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

bool GameSession::OnRecv(PacketHeader* _packet)
{
	auto temp = reinterpret_cast<TestPacket*>(_packet);

	cout << "OnRecv Len " << temp->m_dataSize << " / " << temp->m_test << endl;

	auto sender = Sender::Alloc<TestPacket>();
	auto packet = sender->GetWritePointer<TestPacket>();
	packet->m_packetNum = 0;
	packet->m_dataSize = sizeof(TestPacket);
	strcpy_s(packet->m_test, "Test(Server)");
	GSessionManager.Broadcast(sender);

	return true;
}

void GameSession::OnSend(int32 _len)
{
	cout << "OnSend Len " << _len << endl;
}