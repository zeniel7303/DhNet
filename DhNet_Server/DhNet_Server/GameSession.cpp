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
	if (m_myPrivKey)
		EVP_PKEY_free(m_myPrivKey);
	LOG_DEBUG("~GameSession");
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
	if (!m_handshakeDone.load(std::memory_order_acquire))
	{
		if (_packet->m_packetNum == PacketEnum::Req_KeyExchange)
		{
			HandleKeyExchange(reinterpret_cast<ReqKeyExchange*>(_packet));
			return true;
		}
		Disconnect(L"ProtocolViolation");
		return false;
	}

	if (_packet->m_packetNum != PacketEnum::Encrypted)
	{
		Disconnect(L"ProtocolViolation");
		return false;
	}

	auto* enc = reinterpret_cast<EncryptedPacket*>(_packet);
	int32 payloadLen = enc->m_dataSize - static_cast<int32>(sizeof(PacketHeader));
	if (payloadLen < 28 || payloadLen > MAX_PLAIN_PACKET_SIZE + 28)
	{
		Disconnect(L"InvalidEncryptedSize");
		return false;
	}

	uint8 plainBuf[MAX_PLAIN_PACKET_SIZE];
	int32 plainLen = 0;
	if (!m_cipher->Decrypt(enc->payload, payloadLen, plainBuf, plainLen))
	{
		Disconnect(L"GCMVerifyFailed");
		return false;
	}

	auto* inner = reinterpret_cast<PacketHeader*>(plainBuf);
	return PacketHandler::Instance().Process(inner->m_packetNum, inner, std::static_pointer_cast<Session>(shared_from_this()));
}

void GameSession::Send(SenderRef _sender)
{
	if (m_handshakeDone.load(std::memory_order_acquire))
		SendEncrypted(std::move(_sender));
	else
		Session::Send(std::move(_sender));
}

void GameSession::HandleKeyExchange(ReqKeyExchange* _pkt)
{
	m_myPrivKey = EcdhKeyExchange::GenerateKeyPair();
	if (!m_myPrivKey)
	{
		Disconnect(L"KeyGenFailed");
		return;
	}

	uint8 secret[32];
	if (!EcdhKeyExchange::DeriveSharedSecret(m_myPrivKey, _pkt->clientPubKey, secret))
	{
		SecureZeroMemory(secret, sizeof(secret));
		Disconnect(L"EcdhFailed");
		return;
	}

	uint8 sessionKey[16];
	if (!EcdhKeyExchange::DeriveSessionKey(secret, sessionKey))
	{
		SecureZeroMemory(secret, sizeof(secret));
		SecureZeroMemory(sessionKey, sizeof(sessionKey));
		Disconnect(L"KeyDeriveFailed");
		return;
	}

	auto [res, sender] = Sender::GetSenderAndPacket<ResKeyExchange>();
	if (!EcdhKeyExchange::GetPublicKeyBytes(m_myPrivKey, res->serverPubKey))
	{
		SecureZeroMemory(secret, sizeof(secret));
		SecureZeroMemory(sessionKey, sizeof(sessionKey));
		Disconnect(L"PubKeyExportFailed");
		return;
	}
	res->Init();

	EVP_PKEY_free(m_myPrivKey);
	m_myPrivKey = nullptr;

	m_cipher = std::make_unique<AesGcm>();
	m_cipher->Init(sessionKey);
	SecureZeroMemory(secret, sizeof(secret));
	SecureZeroMemory(sessionKey, sizeof(sessionKey));
	m_handshakeDone.store(true, std::memory_order_release);

	Session::Send(std::move(sender));
}

void GameSession::SendEncrypted(SenderRef _inner)
{
	auto* innerRaw = reinterpret_cast<const uint8*>(_inner->GetSendPointer());
	int32 innerLen  = _inner->GetSendSize();

	if (innerLen <= 0 || innerLen > MAX_PLAIN_PACKET_SIZE)
		return;

	auto [enc, encSender] = Sender::GetSenderAndPacket<EncryptedPacket>();
	if (!encSender)
		return;

	int32 payloadLen = 0;
	if (!m_cipher->Encrypt(innerRaw, innerLen, enc->payload, payloadLen))
		return;

	enc->Init(PacketEnum::Encrypted,
	          static_cast<uint16>(sizeof(PacketHeader) + payloadLen));

	Session::Send(std::move(encSender));
}

void GameSession::OnSend(int32 _len)
{
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