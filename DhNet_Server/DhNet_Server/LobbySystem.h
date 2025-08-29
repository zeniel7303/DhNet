#pragma once
#include <future>

#include "Lobby.h"
#include "Room.h"

class LobbySystem
{
private:
    std::shared_ptr<Lobby> m_lobby;

    // Room management moved here from RoomSystem
    USE_LOCK
    std::map<int, std::shared_ptr<Room>> m_rooms;
    std::atomic<int> m_roomNum;

public:
    LobbySystem();
    ~LobbySystem() = default;

    std::shared_ptr<Lobby> GetLobby();
    void EnterLobby(std::shared_ptr<Player> _player);
    void LeaveLobby(std::shared_ptr<Player> _player);
    std::future<void> EnterLobbyAsync(std::shared_ptr<Player> _player);
    std::future<void> LeaveLobbyAsync(std::shared_ptr<Player> _player);

    std::shared_ptr<Room> MakeRoom();
    std::shared_ptr<Room> GetRoom(int32 roomIndex = 0);
    std::map<int, std::shared_ptr<Room>> GetRooms();
    std::shared_ptr<Room> GetNotFullRoom();
};
