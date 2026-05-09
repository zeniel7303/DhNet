#include "stdafx.h"
#include "../ServerCore/PacketHandler.h"
#include "GameServer.h"
#include "GameSession.h"
#include "Lobby.h"
#include "../DhNet_Protocol/PacketList.h"

GameSession::GameSession()
{
	
}

GameSession::~GameSession()
{
	std::cout << "~GameSession" << std::endl;
}

void GameSession::OnConnected()
{
	GameServer::Instance().GetSystem<GameSessionSystem>()->Add(std::static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	if (m_player)
	{
		m_player->LeaveLobby();
		m_player->LeaveRoom();
		GameServer::Instance().GetSystem<PlayerSystem>()->Remove(m_player);
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

void GameSession::OnLoginResult(bool _ok, uint64 _accountId, std::string _playerName)
{
    // Atomic exchange prevents duplicate processing if two login responses race
    // on the GlobalQueue (5 IOCP worker threads process it concurrently)
    bool expected = false;
    if (!m_loginProcessed.compare_exchange_strong(expected, true))
        return;

    if (!IsConnected())
        return;

    if (!_ok)
    {
        auto senderAndPacket = Sender::GetSenderAndPacket<ResLoginFailed>();
        senderAndPacket.first->Init();
        Send(senderAndPacket.second);
        Disconnect(L"LoginFailed");
        return;
    }

    auto self   = std::static_pointer_cast<GameSession>(shared_from_this());
    auto player = std::make_shared<Player>(self, _accountId, _playerName);

    SetPlayer(player);
    GameServer::Instance().GetSystem<PlayerSystem>()->Add(player);

    auto senderAndPacket = Sender::GetSenderAndPacket<ResLogin>();
    senderAndPacket.first->Init(player->GetPlayerId(), player->GetPlayerName());
    Send(senderAndPacket.second);

    auto lobby = GameServer::Instance().GetSystem<LobbySystem>()->AssignLobby();
    if (lobby)
        lobby->DoAsync(lobby, &Lobby::Enter, player);
}