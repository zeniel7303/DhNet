#pragma once
#include "../../DhNet_Server/ServerCore/Session.h"
#include "../../DhNet_Server/ServerCore/PacketHandler.h"

#include "../../DhNet_Server/DhNet_Protocol/PacketList.h"
#include "../../DhNet_Server/DhNet_Protocol/PacketEnum.h"

#include "../../DhNet_Server/DhNet_Server/AesGcm.h"
#include "../../DhNet_Server/DhNet_Server/EcdhKeyExchange.h"

class ServerSession : public Session
{
private:
	uint64			m_playerId = 0;
	std::string		m_playerName;

	// ── 암호화 상태 ──────────────────────────────────────
	std::unique_ptr<AesGcm>		m_cipher;
	std::atomic<bool>			m_handshakeDone{ false };
	EVP_PKEY*					m_myPrivKey{ nullptr };

public:
	uint64			GetPlayerId()   const { return m_playerId; }
	std::string		GetPlayerName() const { return m_playerName; }
	void			SetPlayerId(uint64 id)               { m_playerId = id; }
	void			SetPlayerName(const std::string& name) { m_playerName = name; }

	~ServerSession()
	{
		if (m_myPrivKey) EVP_PKEY_free(m_myPrivKey);
		std::cout << "~ServerSession()" << std::endl;
	}

	virtual void OnConnected() override;
	virtual bool OnRecv(PacketHeader* _packet) override;
	virtual void OnSend(int32 _len) override;
	virtual void OnDisconnected() override;
	virtual void Send(SenderRef _sender) override;

private:
	void HandleResKeyExchange(ResKeyExchange* _pkt);
	void SendEncrypted(SenderRef _inner);
};
