#include "stdafx.h"
#include "../ServerCore/PacketHandler.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(std::static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()  
{  
   GSessionManager.Remove(std::static_pointer_cast<GameSession>(shared_from_this()));

   if (m_player)
   {
	   m_player->LeaveRoom();
   }

   m_player.reset();
}

bool GameSession::OnRecv(PacketHeader* _packet)
{
	return PacketHandler::Instance().Process(_packet->m_packetNum, _packet, std::static_pointer_cast<Session>(shared_from_this()));
}

void GameSession::OnSend(int32 _len)
{
	std::cout << "OnSend Len " << _len << std::endl;
}