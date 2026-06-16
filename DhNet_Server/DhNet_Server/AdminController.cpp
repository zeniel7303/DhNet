#include "stdafx.h"
#include "AdminController.h"

#ifdef DHNET_GRPC_AVAILABLE
#include <chrono>
#include "AdminGrpcServer.h"
#include "RoomSystem.h"
#include "PlayerSystem.h"
#include "Player.h"
#include "Lobby.h"
#include "LobbySystem.h"
#include "GameServer.h"
#include "../DhNet_Protocol/PacketList.h"
#include "../ServerCore/Sender.h"

bool AdminHealthCheck(dhnet::HealthCheckResponse* resp)
{
    if (!resp) return false;
    resp->set_status("OK");
    return true;
}

bool AdminListRooms(const dhnet::ListRoomsRequest* /*req*/, dhnet::ListRoomsResponse* resp, std::string& err)
{
    return DispatchToLogicThreadWithTimeout([resp]() -> bool {
        if (!resp) return false;

        auto rooms = GameServer::Instance().GetSystem<RoomSystem>()->GetRooms();
        if (rooms.empty()) return false;

        for (const auto& kv : rooms)
        {
            const auto& room = kv.second;
            if (!room) continue;

            dhnet::RoomInfo* r = resp->add_rooms();
            const auto id = static_cast<int64_t>(room->GetRoomIndex());
            r->set_id(id);
            r->set_name("room-" + std::to_string(id));
            r->set_playercount(room->GetPlayerCount());
            r->set_capacity(MAX_ROOM_PLAYER);
        }
        return true;
    }, std::chrono::milliseconds(1000), err);
}

bool AdminBroadcast(const dhnet::BroadcastRequest* req, dhnet::BroadcastResponse* resp, std::string& err)
{
    const auto roomId = req ? req->roomid() : 0;
    const auto message = req ? req->message() : std::string();

    return DispatchToLogicThreadWithTimeout([roomId, message, resp]() -> bool {
        if (!resp) return false;
        if (message.empty())
        {
            resp->set_success(false);
            resp->set_detail("Message is empty");
            return true;
        }

        auto room = GameServer::Instance().GetSystem<RoomSystem>()->GetRoom(static_cast<int32>(roomId));
        if (!room)
        {
            resp->set_success(false);
            resp->set_detail("Room not available");
            return true;
        }

        auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomChat>();
        senderAndPacket.first->Init(0, "ADMIN", message.c_str());
        room->DoAsync(room, &Room::Broadcast, senderAndPacket.second);

        resp->set_success(true);
        resp->set_detail("OK");
        return true;
    }, std::chrono::milliseconds(1500), err);
}

bool AdminListPlayers(const dhnet::ListPlayersRequest* /*req*/, dhnet::ListPlayersResponse* resp, std::string& err)
{
    return DispatchToLogicThreadWithTimeout([resp]() -> bool {
        if (!resp) return false;

        auto players = GameServer::Instance().GetSystem<PlayerSystem>()->GetPlayers();
        for (const auto& player : players)
        {
            if (!player) continue;

            dhnet::PlayerInfo* p = resp->add_players();
            p->set_id(player->GetPlayerId());
            p->set_name(player->GetPlayerName());

            int32 lobbyIndex = -1;
            if (auto lobby = player->GetCurrentLobby().lock())
                lobbyIndex = lobby->GetLobbyIndex();
            p->set_lobbyindex(lobbyIndex);

            int32 roomIndex = -1;
            if (auto room = player->GetCurrentRoom().lock())
                roomIndex = room->GetRoomIndex();
            p->set_roomindex(roomIndex);
        }
        return true;
    }, std::chrono::milliseconds(1000), err);
}

bool AdminKickPlayer(const dhnet::KickPlayerRequest* req, dhnet::KickPlayerResponse* resp, std::string& err)
{
    const auto playerId = req ? req->id() : 0;

    return DispatchToLogicThreadWithTimeout([playerId, resp]() -> bool {
        if (!resp) return false;

        auto player = GameServer::Instance().GetSystem<PlayerSystem>()->Find(playerId);
        if (!player)
        {
            resp->set_success(false);
            resp->set_detail("Player not found");
            return true;
        }

        if (auto session = player->GetOwnerSession())
            session->Disconnect(L"Kicked by admin");

        resp->set_success(true);
        resp->set_detail("OK");
        return true;
    }, std::chrono::milliseconds(1000), err);
}

bool AdminListLobbies(const dhnet::ListLobbiesRequest* /*req*/, dhnet::ListLobbiesResponse* resp, std::string& err)
{
    return DispatchToLogicThreadWithTimeout([resp]() -> bool {
        if (!resp) return false;

        auto lobbies = GameServer::Instance().GetSystem<LobbySystem>()->GetLobbies();
        for (const auto& lobby : lobbies)
        {
            if (!lobby) continue;

            dhnet::LobbyInfo* l = resp->add_lobbies();
            l->set_id(lobby->GetLobbyIndex());
            l->set_playercount(lobby->GetPlayerCount());
            l->set_capacity(MAX_LOBBY_PLAYERS);
        }
        return true;
    }, std::chrono::milliseconds(1000), err);
}

#endif