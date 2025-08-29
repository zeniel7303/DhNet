#include "stdafx.h"
#include "../ServerCore/PacketHandler.h"
#include "GameServer.h"
#include "GameSession.h"

GameSession::GameSession()
{
	
}

GameSession::~GameSession()
{
	// std::cout << "~GameSession" << std::endl;
}

void GameSession::OnConnected()
{
	GameServer::Instance().GetSystem<GameSessionSystem>()->Add(std::static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()  
{  
	if (m_player)
	{
		m_player->DisConnect();
		
	}

	m_player.reset();

	GameServer::Instance().GetSystem<GameSessionSystem>()->Remove(std::static_pointer_cast<GameSession>(shared_from_this()));
}

bool GameSession::OnRecv(PacketHeader* _packet)
{
	return PacketHandler::Instance().Process(_packet->m_packetNum, _packet, std::static_pointer_cast<Session>(shared_from_this()));
}

void GameSession::OnSend(int32 _len)
{
	// std::cout << "OnSend Len " << _len << std::endl;
}