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

int32 GameSession::OnRecv(BYTE* _buffer, int32 _len)
{
	cout << "OnRecv Len " << _len << endl;

	/*shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);
	sendBuffer->CopyData(_buffer, _len);

	GSessionManager.Broadcast(sendBuffer);*/

	auto sender = Sender::Alloc<TestPacket>();
	auto packet = sender->GetWritePointer<TestPacket>();
	packet->m_packetNum = 0;
	strcpy_s(packet->m_test, "Test");

	GSessionManager.Broadcast(sender);

	return _len;
}

void GameSession::OnSend(int32 _len)
{
	cout << "OnSend Len " << _len << endl;
}