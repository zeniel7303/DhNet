#include "stdafx.h"
#include "AdminController.h"

#ifdef DHNET_GRPC_AVAILABLE
#include <chrono>
#include "AdminGrpcServer.h"
#include "RoomSystem.h"

bool AdminHealthCheck(dhnet::HealthCheckResponse* resp)
{
    if (!resp) return false;
    resp->set_status("OK");
    return true;
}

bool AdminListRooms(const dhnet::ListRoomsRequest* /*req*/, dhnet::ListRoomsResponse* resp, std::string& err)
{
    return DispatchToLogicThreadWithTimeout([resp]() -> bool {
        dhnet::RoomInfo* r = resp->add_rooms();
        r->set_id(1);
        r->set_name("room list test");
        r->set_playercount(1557);
        r->set_capacity(888484);
        return true;
    }, std::chrono::milliseconds(1000), err);
}

bool AdminBroadcast(const dhnet::BroadcastRequest* req, dhnet::BroadcastResponse* resp, std::string& err)
{
    const auto roomId = req ? req->roomid() : 0;
    const auto message = req ? req->message() : std::string();

    return DispatchToLogicThreadWithTimeout([roomId, message, resp]() -> bool {
        (void)roomId;
        (void)message;
        resp->set_success(true);
        resp->set_detail(message);
        return true;
    }, std::chrono::milliseconds(1500), err);
}

#endif
