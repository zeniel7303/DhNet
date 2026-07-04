#pragma once
#include <chrono>
#include <memory>

#include "../../DhNet_Server/ServerCore/PacketHeader.h"
#include "../../DhNet_Server/ServerCore/Session.h"

// Res_LobbyEnter는 로비에 입장한 세션 본인에게만 단독 전송되므로 자기 ID 매칭이 필요 없다.
bool Scenario_HandleResLobbyEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);

// Noti_RoomEnter는 룸의 전원에게 브로드캐스트되므로 자기 ID와 일치할 때만 상태를 전환한다.
bool Scenario_HandleNotiRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);

// Noti_LobbyChat/Noti_RoomChat/Noti_RoomExit/Noti_LobbyPlayerEnter/Noti_LobbyPlayerExit는
// 로비/룸 멤버들에게 브로드캐스트된다(LobbyChat/RoomChat은 본인 포함, PlayerEnter/Exit/
// RoomExit는 동시 접속 중인 다른 세션이 있을 때 그쪽으로 전달됨). StressTest 시나리오는
// 이 알림에 반응할 필요가 없지만, 등록되지 않은 패킷을 받으면 PacketHandler::Process가
// false를 반환하고 이는 Session::ProcessRecv가 "OnRead Error"로 간주해 세션을 스스로
// 끊어버리는 원인이 된다 — 그래서 반드시 등록은 하되 아무 것도 하지 않는다.
bool Scenario_HandleIgnoredBroadcastPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);

// 메인 루프에서 세션마다 주기적으로 호출 — 현재 시나리오 단계의 경과 시간을 확인해
// 다음 패킷(로비 채팅/룸 입장/룸 채팅/룸 퇴장)을 보낼지 결정한다.
void AdvanceScenario(const std::shared_ptr<Session>& _session, std::chrono::steady_clock::time_point _now);

// PacketEnum::Test 응답 — 시나리오와 무관한 RTT 측정용 핑의 pong. RTT를 계산해
// MetricsAggregator에 기록하고 pingInFlight를 해제한다.
bool Scenario_HandleResTestPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);

// 메인 루프에서 세션마다 주기적으로 호출 — AdvanceScenario와 별개로 일정 간격마다
// Test 패킷을 보내 RTT를 측정하고, 타임아웃 시 MetricsAggregator에 기록한다.
void AdvancePing(const std::shared_ptr<Session>& _session, std::chrono::steady_clock::time_point _now);
