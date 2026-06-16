#include "stdafx.h"
#include "Lobby.h"
#include "GameSession.h"
#include "../DhNet_Protocol/PacketList.h"

void Lobby::Enter(std::shared_ptr<Player> _player)
{
	// 기존 멤버에게만 신규 입장 알림 (try_emplace 전 브로드캐스트)
	auto notiSender = Sender::GetSenderAndPacket<NotiLobbyPlayerEnter>();
	notiSender.first->Init(_player->GetPlayerId(), _player->GetPlayerName().c_str());
	Broadcast(notiSender.second);

	auto [it, result] = m_players.try_emplace(_player->GetPlayerId(), _player);
	if (!result)
	{
		ReleaseReservedSlot();
		return;
	}

	const auto self = std::static_pointer_cast<Lobby>(shared_from_this());
	_player->SetCurrentLobby(self);

	auto session = _player->GetOwnerSession();
	if (!session) return;

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
