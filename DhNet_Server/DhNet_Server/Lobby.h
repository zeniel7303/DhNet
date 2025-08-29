#pragma once
#include "GameSession.h"
#include "Player.h"
#include "../../DhUtil/JobQueue.h"

class Lobby : public JobQueue
{
private:
    std::map<uint64, std::shared_ptr<Player>> m_players;

public:
    Lobby() = default;
    ~Lobby() = default;

    void Enter(std::shared_ptr<Player> _player);
    void Leave(std::shared_ptr<Player> _player);
    void Broadcast(std::shared_ptr<Sender> _sender);

    int32 GetPlayerCount() const { return static_cast<int32>(m_players.size()); }
};
