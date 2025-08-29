#include "stdafx.h"
#include "Lobby.h"

void Lobby::Enter(std::shared_ptr<Player> _player)
{
    m_players.emplace(_player->GetPlayerId(), _player);

    std::cout << m_players.size() << std::endl;
}

void Lobby::Leave(std::shared_ptr<Player> _player)
{
    auto it = m_players.find(_player->GetPlayerId());
    if (it != m_players.end())
    {
        m_players.erase(it);
    }

    std::cout << m_players.size() << std::endl;
}

void Lobby::Broadcast(std::shared_ptr<Sender> _sender)
{
    for (auto& p : m_players)
    {
        p.second->GetOwnerSession()->Send(_sender);
    }
}
