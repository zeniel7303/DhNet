#include "stdafx.h"
#include "Lobby.h"
#include "GameSession.h"
#include "../DhNet_Protocol/PacketList.h"

void Lobby::Enter(std::shared_ptr<Player> _player)
{
	auto [it, result] = m_players.try_emplace(_player->GetPlayerId(), _player);
	if (!result)
	{
		ReleaseReservedSlot();
		return;
	}

	const auto self = std::static_pointer_cast<Lobby>(shared_from_this());
	_player->SetCurrentLobby(self);

	auto session = _player->GetOwnerSession();
	if (!session)
	{
		// Enter 실행 전 연결이 끊긴 경우: 아직 알림 미전송이므로 조용히 정리
		m_players.erase(_player->GetPlayerId());
		ReleaseReservedSlot();
		return;
	}

	// 세션 유효성 확인 후 기존 멤버에게만 입장 알림 (신규 플레이어 본인 제외)
	auto notiSender = Sender::GetSenderAndPacket<NotiLobbyPlayerEnter>();
	notiSender.first->Init(_player->GetPlayerId(), _player->GetPlayerName().c_str());
	for (auto& [id, player] : m_players)
	{
		if (id == _player->GetPlayerId()) continue;
		if (auto sess = player->GetOwnerSession())
			sess->Send(notiSender.second);
	}

	auto senderAndPacket = Sender::GetSenderAndPacket<ResLobbyEnter>();
	senderAndPacket.first->Init(m_lobbyIndex);
	for (auto& [id, player] : m_players)
	{
		senderAndPacket.first->AddPlayer(player->GetPlayerId(), player->GetPlayerName().c_str());
	}
	session->Send(senderAndPacket.second);
}

void Lobby::Exit(std::shared_ptr<Player> _player)
{
	auto it = m_players.find(_player->GetPlayerId());
	if (it == m_players.end()) return;

	m_players.erase(it);
	m_availableSlots.fetch_add(1);

	auto senderAndPacket = Sender::GetSenderAndPacket<NotiLobbyPlayerExit>();
	senderAndPacket.first->Init(_player->GetPlayerId(), _player->GetPlayerName());
	Broadcast(senderAndPacket.second);
}

void Lobby::Chat(std::shared_ptr<Player> _player, std::string _message)
{
	auto senderAndPacket = Sender::GetSenderAndPacket<NotiLobbyChat>();
	senderAndPacket.first->Init(_player->GetPlayerId(), _player->GetPlayerName(), _message.c_str());
	Broadcast(senderAndPacket.second);
}

void Lobby::Broadcast(std::shared_ptr<Sender> _sender)
{
	for (auto& [id, player] : m_players)
	{
		if (auto session = player->GetOwnerSession())
			session->Send(_sender);
	}
}

bool Lobby::TryReserveSlot()
{
	while (true)
	{
		int32 avail = m_availableSlots.load();
		if (avail <= 0) return false;

		if (m_availableSlots.compare_exchange_weak(avail, avail - 1))
			return true;
	}
}

void Lobby::ReleaseReservedSlot()
{
	m_availableSlots.fetch_add(1);
}
