#pragma once
#include "Lobby.h"

class LobbySystem
{
private:
	USE_LOCK
	std::vector<std::shared_ptr<Lobby>> m_lobbies;

public:
	LobbySystem();
	~LobbySystem() = default;

	std::shared_ptr<Lobby> AssignLobby();
	std::vector<std::shared_ptr<Lobby>> GetLobbies();
};
