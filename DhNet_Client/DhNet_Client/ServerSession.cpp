#include "stdafx.h"
#include "ServerSession.h"

void ServerSession::OnConnected()
{
	std::cout << "Connected To Server" << std::endl;

	m_myPrivKey = EcdhKeyExchange::GenerateKeyPair();
	if (!m_myPrivKey)
	{
		Disconnect(L"KeyGenFailed");
		return;
	}

	auto [pkt, sender] = Sender::GetSenderAndPacket<ReqKeyExchange>();
	if (!EcdhKeyExchange::GetPublicKeyBytes(m_myPrivKey, pkt->clientPubKey))
	{
		Disconnect(L"PubKeyExportFailed");
		return;
	}
	pkt->Init();
	Session::Send(std::move(sender));

	if (m_onConnectedExtra) m_onConnectedExtra();
}

bool ServerSession::OnRecv(PacketHeader* _packet)
{
	if (!m_handshakeDone.load(std::memory_order_acquire))
	{
		if (_packet->m_packetNum == PacketEnum::Res_KeyExchange)
		{
			HandleResKeyExchange(reinterpret_cast<ResKeyExchange*>(_packet));
			return true;
		}
		return false;
	}

	if (_packet->m_packetNum != PacketEnum::Encrypted)
		return false;

	auto* enc = reinterpret_cast<EncryptedPacket*>(_packet);
	int32 payloadLen = enc->m_dataSize - static_cast<int32>(sizeof(PacketHeader));
	if (payloadLen < 28 || payloadLen > MAX_PLAIN_PACKET_SIZE + 28)
		return false;

	uint8 plainBuf[MAX_PLAIN_PACKET_SIZE];
	int32 plainLen = 0;
	if (!m_cipher->Decrypt(enc->payload, payloadLen, plainBuf, plainLen))
		return false;

	auto* inner = reinterpret_cast<PacketHeader*>(plainBuf);
	return PacketHandler::Instance().Process(inner->m_packetNum, inner,
	           std::static_pointer_cast<Session>(shared_from_this()));
}

void ServerSession::Send(SenderRef _sender)
{
	if (m_handshakeDone.load(std::memory_order_acquire))
		SendEncrypted(std::move(_sender));
	else
		Session::Send(std::move(_sender));
}

void ServerSession::HandleResKeyExchange(ResKeyExchange* _pkt)
{
	uint8 secret[32];
	if (!EcdhKeyExchange::DeriveSharedSecret(m_myPrivKey, _pkt->serverPubKey, secret))
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

	EVP_PKEY_free(m_myPrivKey);
	m_myPrivKey = nullptr;

	m_cipher = std::make_unique<AesGcm>();
	m_cipher->Init(sessionKey);
	SecureZeroMemory(secret, sizeof(secret));
	SecureZeroMemory(sessionKey, sizeof(sessionKey));
	m_handshakeDone.store(true, std::memory_order_release);

	std::cout << "[Crypto] Handshake complete. Session key established.\n";

	// 핸드셰이크 완료 → 첫 번째 암호화 패킷으로 로그인 전송
	auto [pkt, sender] = Sender::GetSenderAndPacket<ReqLogin>();
	pkt->Init("testuser", "testpass");
	Send(std::move(sender));
}

void ServerSession::SendEncrypted(SenderRef _inner)
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

void ServerSession::OnSend(int32 _len)
{
	// std::cout << "OnSend Len = " << _len << std::endl;
}

void ServerSession::OnDisconnected()
{
	std::cout << "Disconnected" << std::endl;

	if (m_onDisconnectedExtra) m_onDisconnectedExtra();
}