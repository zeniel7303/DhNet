#pragma once
#include <chrono>

// 서버 측 Res_RoomEnter/Res_RoomExit는 실제로 전송되지 않으므로(Player::EnterRoom 등에서 주석 처리됨),
// 룸 입장/퇴장 확인은 Noti_RoomEnter(자기 자신 매칭)와 Res_LobbyEnter 수신으로만 가능하다.
enum class ScenarioPhase
{
    Handshaking,
    InLobby,
    AwaitingRoomEnter,
    InRoom,
    AwaitingRoomExit,
    Stuck, // 재시도 한도 초과 — 서버 응답 누락 또는 race 의심, 더 이상 진행하지 않음
};

// AdvanceScenario가 락 보유 중에는 상태만 변경하고, 실제 Send()는 락 해제 후
// 이 액션을 보고 수행한다 (네트워크 syscall을 레지스트리 락 안에서 호출하지 않기 위함).
enum class ScenarioAction
{
    None,
    SendLobbyChat,
    SendRoomEnter,
    SendRoomChat,
    SendRoomExit,
    LogStuckRoomEnter,
    LogStuckRoomExit,
};

struct ScenarioState
{
    ScenarioPhase phase = ScenarioPhase::Handshaking;
    std::chrono::steady_clock::time_point phaseStartTime{};
    std::chrono::steady_clock::time_point lastActionTime{};
    int lobbyChatsSent = 0;
    int roomChatsSent = 0;
    int actionRetries = 0; // AwaitingRoomEnter/AwaitingRoomExit 타임아웃 재시도 횟수

    // RTT 측정용 핑 상태 — 시나리오 단계와 무관하게 독립적으로 진행됨
    bool pingInFlight = false;
    std::chrono::steady_clock::time_point pingSentTime{};
    std::chrono::steady_clock::time_point lastPingSentTime{};
};
