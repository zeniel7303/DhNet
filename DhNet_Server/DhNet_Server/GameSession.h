#pragma once
#include "../ServerCore/Session.h"
#include "Player.h"

class GameSession : public Session
{
private:
	std::shared_ptr<Player>		m_player;
	std::atomic<bool>			m_loginProcessed{ false };

public:
	GameSession();
	~GameSession();

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual bool OnRecv(PacketHeader* _packet) override;
	virtual void OnSend(int32 _len) override;

	void SetPlayer(std::shared_ptr<Player> _player)
	{
		m_player = _player;
	}

	std::shared_ptr<Player> GetPlayer() { return m_player; }

	// Called on a logic thread after DB authentication completes
	void OnLoginResult(bool _ok, uint64 _accountId, std::string _playerName);
};