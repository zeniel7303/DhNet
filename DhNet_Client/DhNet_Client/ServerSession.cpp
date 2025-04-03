#include "stdafx.h"
#include "ServerSession.h"

void ServerSession::OnConnected()
{
	std::cout << "Connected To Server" << std::endl;

	auto senderAndPacket = Sender::GetSenderAndPacket<ReqLogin>();
	senderAndPacket.first->Init(PacketEnum::Req_Login, sizeof(ReqLogin));
	Send(senderAndPacket.second);
}

bool ServerSession::OnRecv(PacketHeader* _packet)
{
	return PacketHandler::Instance().Process(_packet->m_packetNum, _packet, std::static_pointer_cast<Session>(shared_from_this()));
}

void ServerSession::OnSend(int32 _len)
{
	// std::cout << "OnSend Len = " << _len << std::endl;
}

void ServerSession::OnDisconnected()
{
	std::cout << "Disconnected" << std::endl;
}