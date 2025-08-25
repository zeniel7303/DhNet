#include "stdafx.h"
#include "AdminGrpcServer.h"
#include <future>
#include <chrono>
#include "../../DhUtil/ThreadManager.h"
#include "RoomSystem.h"

GrpcHost& GrpcHost::Instance()
{
    static GrpcHost s;
    return s;
}

bool GrpcHost::Start(const std::string& address)
{
#ifdef DHNET_GRPC_AVAILABLE
    if (m_running.exchange(true)) return false;

    auto service = std::make_unique<AdminServiceImpl>();

    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(service.get());

    m_server = builder.BuildAndStart();
    if (!m_server)
    {
        m_running = false;
        return false;
    }

    m_thread = std::thread([this, svc = std::move(service)]() mutable {
        (void)svc;
        m_server->Wait();
    });

    return true;
#else
    (void)address;
    m_running = true;
    return true;
#endif
}

void GrpcHost::Stop()
{
#ifdef DHNET_GRPC_AVAILABLE
    if (!m_running.exchange(false)) return;
    
    if (m_server) {
        m_server->Shutdown();
        m_server.reset();
    }
    
    if (m_thread.joinable()) m_thread.join();
#else
    m_running = false;
#endif
}

#ifdef DHNET_GRPC_AVAILABLE
grpc::Status AdminServiceImpl::HealthCheck(grpc::ServerContext*, const dhnet::HealthCheckRequest*, dhnet::HealthCheckResponse* resp)
{
    resp->set_status("OK");
    return grpc::Status::OK;
}

template<typename Fn>
bool AdminServiceImpl::DispatchToLogicThreadWithTimeout(Fn&& fn, std::chrono::milliseconds timeout, std::string& err)
{
    auto prom = std::make_shared<std::promise<bool>>();
    auto fut = prom->get_future();

    ThreadManager::PushGlobalQueue([fn = std::forward<Fn>(fn), prom]() mutable {
        try
        {
            bool ok = fn();
            prom->set_value(ok);
        }
        catch (...) {
            prom->set_value(false);
        }
    });

    if (fut.wait_for(timeout) == std::future_status::ready)
    {
        return fut.get();
    }
    else
    {
        err = "Logic thread dispatch timeout";
        return false;
    }
}

grpc::Status AdminServiceImpl::ListRooms(grpc::ServerContext*, const dhnet::ListRoomsRequest*, dhnet::ListRoomsResponse* resp)
{
    std::string err;
    bool ok = DispatchToLogicThreadWithTimeout([resp]() -> bool {
        dhnet::RoomInfo* r = resp->add_rooms();
        r->set_id(1);
        r->set_name("room list test");
        r->set_playercount(1557);
        r->set_capacity(888484);
        return true;
    }, std::chrono::milliseconds(1000), err);

    if (!ok)
    {
        return grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED, err);
    }
    
    return grpc::Status::OK;
}

grpc::Status AdminServiceImpl::Broadcast(grpc::ServerContext*, const dhnet::BroadcastRequest* req, dhnet::BroadcastResponse* resp)
{
    std::string err;
    const auto roomId = req->roomid();
    const auto message = req->message();

    bool ok = DispatchToLogicThreadWithTimeout([roomId, message, resp]() -> bool {
        (void)roomId;
        (void)message;
        resp->set_success(true);
        resp->set_detail(message);
        return true;
    }, std::chrono::milliseconds(1500), err);

    if (!ok)
    {
        resp->set_success(false);
        resp->set_detail(err);
        return grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED, err);
    }

    return grpc::Status::OK;
}
#endif