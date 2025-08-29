#include "stdafx.h"
#include "LobbySystem.h"
#include "../../DhUtil/ObjectPool.h"

LobbySystem::LobbySystem() : m_roomNum(0)
{
    m_lobby = ObjectPool<Lobby>::MakeShared();
}

std::shared_ptr<Lobby> LobbySystem::GetLobby()
{
    return m_lobby;
}

void LobbySystem::EnterLobby(std::shared_ptr<Player> _player)
{
    ASSERT_CRASH(m_lobby !=nullptr)
    
    m_lobby->DoAsync(m_lobby, &Lobby::Enter, _player);
}

void LobbySystem::LeaveLobby(std::shared_ptr<Player> _player)
{
    ASSERT_CRASH(m_lobby !=nullptr)
    
    m_lobby->DoAsync(m_lobby, &Lobby::Leave, _player);
}

std::future<void> LobbySystem::EnterLobbyAsync(std::shared_ptr<Player> _player)
{
    ASSERT_CRASH(m_lobby !=nullptr)
    auto pr = std::make_shared<std::promise<void>>();
    auto fut = pr->get_future();

    m_lobby->DoAsync([this, _player, pr]() mutable {
        m_lobby->Enter(_player);
        pr->set_value();
    });

    return fut;
}

std::future<void> LobbySystem::LeaveLobbyAsync(std::shared_ptr<Player> _player)
{
    ASSERT_CRASH(m_lobby !=nullptr)
    
    auto pr = std::make_shared<std::promise<void>>();
    auto fut = pr->get_future();

    m_lobby->DoAsync([this, _player, pr]() mutable {
        m_lobby->Leave(_player);
        pr->set_value();
    });

    return fut;
}

std::shared_ptr<Room> LobbySystem::MakeRoom()
{
    WRITE_LOCK

    auto room = ObjectPool<Room>::MakeShared();
    ASSERT_CRASH(room != nullptr)

    room->SetRoomIndex(m_roomNum);
    m_rooms[m_roomNum] = room;
    m_roomNum.fetch_add(1);
    ASSERT_CRASH(room->TryReserveSlot())

    return room;
}

std::shared_ptr<Room> LobbySystem::GetRoom(int32 roomIndex)
{
    {
        READ_LOCK
        if (const auto it = m_rooms.find(roomIndex); it != m_rooms.end())
        {
            return it->second;
        }
    }

    return nullptr;
}

std::map<int, std::shared_ptr<Room>> LobbySystem::GetRooms()
{
    READ_LOCK
    return m_rooms; // return a copy for safe iteration outside lock
}

std::shared_ptr<Room> LobbySystem::GetNotFullRoom()
{
    READ_LOCK
    for (auto& [index, room] : m_rooms)
    {
        if (room && room->TryReserveSlot())
            return room;
    }
    return nullptr;
}
