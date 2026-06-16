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
    struct RoomEntry { int64_t id; std::string name; int32 playerCount; int32 capacity; };
    auto result = std::make_shared<std::vector<RoomEntry>>();

    bool ok = DispatchToLogicThreadWithTimeout([result]() -> bool {
        auto rooms = GameServer::Instance().GetSystem<RoomSystem>()->GetRooms();
        if (rooms.empty()) return true;

        for (const auto& kv : rooms)
        {
            const auto& room = kv.second;
            if (!room) continue;

            const int64_t id = room->GetRoomIndex();
            result->push_back({ id, "room-" + std::to_string(id), room->GetPlayerCount(), MAX_ROOM_PLAYER });
        }
        return true;
    }, std::chrono::milliseconds(1000), err);

    if (!ok) return false;

    for (const auto& e : *result)
    {
        dhnet::RoomInfo* r = resp->add_rooms();
        r->set_id(e.id);
        r->set_name(e.name);
        r->set_playercount(e.playerCount);
        r->set_capacity(e.capacity);
    }
    return true;
}

bool AdminBroadcast(const dhnet::BroadcastRequest* req, dhnet::BroadcastResponse* resp, std::string& err)
{
    const auto roomId  = req ? req->roomid()  : 0;
    const auto message = req ? req->message() : std::string();

    struct BroadcastResult { bool success; std::string detail; };
    auto result = std::make_shared<BroadcastResult>();

    bool ok = DispatchToLogicThreadWithTimeout([roomId, message, result]() -> bool {
        if (message.empty())
        {
            result->success = false;
            result->detail  = "Message is empty";
            return true;
        }

        auto room = GameServer::Instance().GetSystem<RoomSystem>()->GetRoom(static_cast<int32>(roomId));
        if (!room)
        {
            result->success = false;
            result->detail  = "Room not available";
            return true;
        }

        auto senderAndPacket = Sender::GetSenderAndPacket<NotiRoomChat>();
        senderAndPacket.first->Init(0, "ADMIN", message.c_str());
        room->DoAsync(room, &Room::Broadcast, senderAndPacket.second);

        result->success = true;
        result->detail  = "OK";
        return true;
    }, std::chrono::milliseconds(1500), err);

    if (!ok) return false;

    resp->set_success(result->success);
    resp->set_detail(result->detail);
    return true;
}

bool AdminListPlayers(const dhnet::ListPlayersRequest* /*req*/, dhnet::ListPlayersResponse* resp, std::string& err)
{
    struct PlayerEntry { uint64 id; std::string name; int32 lobbyIndex; int32 roomIndex; };
    auto result = std::make_shared<std::vector<PlayerEntry>>();

    bool ok = DispatchToLogicThreadWithTimeout([result]() -> bool {
        auto players = GameServer::Instance().GetSystem<PlayerSystem>()->GetPlayers();
        for (const auto& player : players)
        {
            if (!player) continue;
            result->push_back({
                player->GetPlayerId(),
                player->GetPlayerName(),
                player->GetCurrentLobbyIndex(),
                player->GetCurrentRoomIndex()
            });
        }
        return true;
    }, std::chrono::milliseconds(1000), err);

    if (!ok) return false;

    for (const auto& e : *result)
    {
        dhnet::PlayerInfo* p = resp->add_players();
        p->set_id(e.id);
        p->set_name(e.name);
        p->set_lobbyindex(e.lobbyIndex);
        p->set_roomindex(e.roomIndex);
    }
    return true;
}

bool AdminKickPlayer(const dhnet::KickPlayerRequest* req, dhnet::KickPlayerResponse* resp, std::string& err)
{
    const auto playerId = req ? req->id() : 0;

    struct KickResult { bool success; std::string detail; };
    auto result = std::make_shared<KickResult>();

    bool ok = DispatchToLogicThreadWithTimeout([playerId, result]() -> bool {
        auto player = GameServer::Instance().GetSystem<PlayerSystem>()->Find(playerId);
        if (!player)
        {
            result->success = false;
            result->detail  = "Player not found";
            return true;
        }

        if (auto session = player->GetOwnerSession())
            session->Disconnect(L"Kicked by admin");

        result->success = true;
        result->detail  = "OK";
        return true;
    }, std::chrono::milliseconds(1000), err);

    if (!ok) return false;

    resp->set_success(result->success);
    resp->set_detail(result->detail);
    return true;
}

bool AdminListLobbies(const dhnet::ListLobbiesRequest* /*req*/, dhnet::ListLobbiesResponse* resp, std::string& err)
{
    struct LobbyEntry { int32 id; int32 playerCount; int32 capacity; };
    auto result = std::make_shared<std::vector<LobbyEntry>>();

    bool ok = DispatchToLogicThreadWithTimeout([result]() -> bool {
        auto lobbies = GameServer::Instance().GetSystem<LobbySystem>()->GetLobbies();
        for (const auto& lobby : lobbies)
        {
            if (!lobby) continue;
            result->push_back({ lobby->GetLobbyIndex(), lobby->GetPlayerCount(), MAX_LOBBY_PLAYERS });
        }
        return true;
    }, std::chrono::milliseconds(1000), err);

    if (!ok) return false;

    for (const auto& e : *result)
    {
        dhnet::LobbyInfo* l = resp->add_lobbies();
        l->set_id(e.id);
        l->set_playercount(e.playerCount);
        l->set_capacity(e.capacity);
    }
    return true;
}

#endif
