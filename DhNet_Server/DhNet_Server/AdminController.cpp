#include "stdafx.h"
#include "AdminController.h"

#ifdef DHNET_GRPC_AVAILABLE
#include <chrono>
#include "AdminGrpcServer.h"
#include "RoomSystem.h"
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

#endif