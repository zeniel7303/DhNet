#pragma once
#include "GameSession.h"
#include "Player.h"
#include "../../DhUtil/JobSerializer.h"

class Room : public JobSerializer
{
private:
	int32 m_roomIndex;
	std::map<uint64, std::shared_ptr<Player>> m_players;

public:
	void Enter(std::shared_ptr<Player> _player);
	void Leave(std::shared_ptr<Player> _player);
	void Broadcast(std::shared_ptr<Sender> _sender);
	
	virtual void FlushJob() override;

	void SetRoomIndex(int32 _roomIndex) { m_roomIndex = _roomIndex; }
};