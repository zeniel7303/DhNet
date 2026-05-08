#pragma once
#include "Player.h"
#include "../../DhUtil/JobQueue.h"

class Lobby : public JobQueue
{
private:
	int32 m_lobbyIndex;
	std::map<uint64, std::shared_ptr<Player>> m_players;
	std::atomic<int32> m_availableSlots{ MAX_LOBBY_PLAYERS };

public:
	Lobby() : m_lobbyIndex(0) {}
	~Lobby() = default;

	void Enter(std::shared_ptr<Player> _player);
	void Exit(std::shared_ptr<Player> _player);
	void Chat(std::shared_ptr<Player> _player, std::string _message);
	void Broadcast(std::shared_ptr<Sender> _sender);

	bool TryReserveSlot();
	void ReleaseReservedSlot();

	int32 GetLobbyIndex() const { return m_lobbyIndex; }
	int32 GetPlayerCount() const { return MAX_LOBBY_PLAYERS - m_availableSlots.load(); }

	void SetLobbyIndex(int32 _lobbyIndex) { m_lobbyIndex = _lobbyIndex; }
};
