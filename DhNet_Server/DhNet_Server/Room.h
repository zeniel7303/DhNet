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

	// Broadcast는 Room 내부에서만 사용한다. 외부에서 직접 호출하지 말 것.
	void Broadcast(std::shared_ptr<Sender> _sender);

public:
	Room() : m_roomIndex(0) {}
	~Room() = default;

	// 아래의 4개의 함수는 반드시 DoAsync를 통해서만 호출되어야 함 (동기 호출 금지)
	void Enter(std::shared_ptr<Player> _player);
	void Leave(std::shared_ptr<Player> _player, bool _isDisConnect);
	void HandleChat(uint64 _playerId, const std::string& _message);
	void AdminBroadcast(const std::string& _message);

	// Thread-safe 하도록 room 입장 예약을 거는 함수
	bool TryReserveSlot();
	void ReleaseReservedSlot();

	int32 GetRoomIndex() const { return m_roomIndex; }
	// 외부에서도 안전하게 쓰일 수 있도록 수정 (but 예약 포함한 근사값을 반환함. 그래도 어지간하면 맞을 것)
	int32 GetPlayerCount() const { return MAX_ROOM_PLAYER - m_availableSlots.load(); }
	// int32 GetPlayerCount() const { return static_cast<int32>(m_players.size()); }
	
	void SetRoomIndex(int32 _roomIndex) { m_roomIndex = _roomIndex; }
};