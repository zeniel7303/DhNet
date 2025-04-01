#include "stdafx.h"
#include "../ServerCore/PacketHandler.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()  
{  
   if (m_player)  
   {  
       m_player->LeaveRoom();  
   }  

   GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));  
}

bool GameSession::OnRecv(PacketHeader* _packet)
{
	return PacketHandler::Instance().Process(_packet->m_packetNum, _packet, static_pointer_cast<Session>(shared_from_this()));
}

void GameSession::OnSend(int32 _len)
{
	cout << "OnSend Len " << _len << endl;
}