#pragma once
#include <chrono>
#include <memory>

#include "../../DhNet_Server/ServerCore/PacketHeader.h"
#include "../../DhNet_Server/ServerCore/Session.h"

// Res_LobbyEnter는 로비에 입장한 세션 본인에게만 단독 전송되므로 자기 ID 매칭이 필요 없다.
bool Scenario_HandleResLobbyEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);

// Noti_RoomEnter는 룸의 전원에게 브로드캐스트되므로 자기 ID와 일치할 때만 상태를 전환한다.
bool Scenario_HandleNotiRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);

// 메인 루프에서 세션마다 주기적으로 호출 — 현재 시나리오 단계의 경과 시간을 확인해
// 다음 패킷(로비 채팅/룸 입장/룸 채팅/룸 퇴장)을 보낼지 결정한다.
void AdvanceScenario(const std::shared_ptr<Session>& _session, std::chrono::steady_clock::time_point _now);

// PacketEnum::Test 응답 — 시나리오와 무관한 RTT 측정용 핑의 pong. RTT를 계산해
// MetricsAggregator에 기록하고 pingInFlight를 해제한다.
bool Scenario_HandleResTestPacket(PacketHeader* _header, std::shared_ptr<Session>& _session);

// 메인 루프에서 세션마다 주기적으로 호출 — AdvanceScenario와 별개로 일정 간격마다
// Test 패킷을 보내 RTT를 측정하고, 타임아웃 시 MetricsAggregator에 기록한다.
void AdvancePing(const std::shared_ptr<Session>& _session, std::chrono::steady_clock::time_point _now);
