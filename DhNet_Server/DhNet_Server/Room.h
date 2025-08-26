#pragma once
#include "GameSession.h"
#include "Player.h"
#include "../../DhUtil/JobQueue.h"

class Room : public JobQueue
{
private:
	int32 m_roomIndex;
	std::map<uint64, std::shared_ptr<Player>> m_players;

public:
	void Enter(std::shared_ptr<Player> _player);
	void Leave(std::shared_ptr<Player> _player);
	void Broadcast(std::shared_ptr<Sender> _sender);

	int32 GetRoomIndex() const { return m_roomIndex; }
	int32 GetPlayerCount() const { return static_cast<int32>(m_players.size()); }
	void SetRoomIndex(int32 _roomIndex) { m_roomIndex = _roomIndex; }
};