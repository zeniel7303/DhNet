#pragma once
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <functional>
#include <chrono>

#if defined(__has_include)
#  if __has_include(<grpcpp/grpcpp.h>) && __has_include("dhnet.grpc.pb.h")
#    define DHNET_GRPC_AVAILABLE 1
#  endif
#endif

#ifdef DHNET_GRPC_AVAILABLE
#include <grpcpp/grpcpp.h>
#include "dhnet.grpc.pb.h"
#endif

#ifdef DHNET_GRPC_AVAILABLE
// Dispatch helper for running work on logic thread with a timeout.
bool DispatchToLogicThreadWithTimeout(const std::function<bool()>& fn,
                                      std::chrono::milliseconds timeout,
                                      std::string& err);
#endif

class GrpcHost
{
private:
    GrpcHost() {}
    ~GrpcHost() {}
    
public:
    static GrpcHost& Instance();

    bool Start(const std::string& address);
    void Stop();

#ifdef DHNET_GRPC_AVAILABLE
    std::unique_ptr<grpc::Server> m_server;
    std::thread m_thread;
    std::atomic<bool> m_running{ false };
#else
    std::atomic<bool> m_running{ false };
#endif
};

#ifdef DHNET_GRPC_AVAILABLE
class AdminServiceImpl final : public dhnet::AdminService::Service
{
public:
    grpc::Status HealthCheck(grpc::ServerContext* ctx, const dhnet::HealthCheckRequest* req,
                             dhnet::HealthCheckResponse* resp) override;

    grpc::Status ListRooms(grpc::ServerContext* ctx, const dhnet::ListRoomsRequest* req,
                           dhnet::ListRoomsResponse* resp) override;

    grpc::Status Broadcast(grpc::ServerContext* ctx, const dhnet::BroadcastRequest* req,
                           dhnet::BroadcastResponse* resp) override;
};
#endif