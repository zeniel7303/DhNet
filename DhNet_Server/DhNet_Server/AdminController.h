#pragma once
#include <string>

#if defined(__has_include)
#  if __has_include(<grpcpp/grpcpp.h>) && __has_include("dhnet.grpc.pb.h")
#    define DHNET_GRPC_AVAILABLE 1
#  endif
#endif

#ifdef DHNET_GRPC_AVAILABLE
#include <grpcpp/grpcpp.h>
#include "dhnet.grpc.pb.h"

// Admin gRPC controller-style free functions mirroring other controllers
// Return true on success; on failure, set err with a human-readable detail when applicable.
bool AdminHealthCheck(dhnet::HealthCheckResponse* resp);
bool AdminListRooms(const dhnet::ListRoomsRequest* req, dhnet::ListRoomsResponse* resp, std::string& err);
bool AdminBroadcast(const dhnet::BroadcastRequest* req, dhnet::BroadcastResponse* resp, std::string& err);
#endif
