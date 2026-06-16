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

bool AdminHealthCheck(dhnet::HealthCheckResponse* resp);
bool AdminListRooms(const dhnet::ListRoomsRequest* req, dhnet::ListRoomsResponse* resp, std::string& err);
bool AdminBroadcast(const dhnet::BroadcastRequest* req, dhnet::BroadcastResponse* resp, std::string& err);
bool AdminListPlayers(const dhnet::ListPlayersRequest* req, dhnet::ListPlayersResponse* resp, std::string& err);
bool AdminKickPlayer(const dhnet::KickPlayerRequest* req, dhnet::KickPlayerResponse* resp, std::string& err);
bool AdminListLobbies(const dhnet::ListLobbiesRequest* req, dhnet::ListLobbiesResponse* resp, std::string& err);
#endif