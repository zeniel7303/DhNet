#include "LoginController.h"

bool HandleResLoginPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	auto resLogin = reinterpret_cast<ResLogin*>(_header);
	auto serverSession = std::static_pointer_cast<ServerSession>(_session);
	serverSession->SetPlayerId(resLogin->m_playerId);
	serverSession->SetPlayerName(resLogin->m_playerName);
	return true;
}

bool HandleResLoginFailedPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
	std::cout << "[LoginFailed] Server rejected credentials\n";
	_session->Disconnect(L"LoginFailed");
	return true;
}
