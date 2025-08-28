#pragma once
#include "GameSession.h"
#include "Player.h"
#include "../../DhUtil/JobQueue.h"

class Room : public JobQueue
{
private:
	int32 m_roomIndex;
	std::map<uint64, std::shared_ptr<Player>> m_players;
	std::atomic<int32> m_availableSlots{ MAX_ROOM_PLAYER };

public:
	Room() : m_roomIndex(0) {}
	~Room() = default;

	void Enter(std::shared_ptr<Player> _player);
	void Leave(std::shared_ptr<Player> _player);
	void Broadcast(std::shared_ptr<Sender> _sender);

	// Thread-safe �ϵ��� room ���� ������ �Ŵ� �Լ�
	bool TryReserveSlot();
	void ReleaseReservedSlot();

	int32 GetRoomIndex() const { return m_roomIndex; }
	// �ܺο����� �����ϰ� ���� �� �ֵ��� ���� (but ���� ������ �ٻ簪�� ��ȯ��. �׷��� �������ϸ� ���� ��)
	int32 GetPlayerCount() const { return MAX_ROOM_PLAYER - m_availableSlots.load(); }
	// int32 GetPlayerCount() const { return static_cast<int32>(m_players.size()); }
	
	void SetRoomIndex(int32 _roomIndex) { m_roomIndex = _roomIndex; }
};