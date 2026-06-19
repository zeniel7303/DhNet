#include "stdafx.h"
#include "ScenarioHandlers.h"

#include "ScenarioStateRegistry.h"
#include "ScenarioConfig.h"
#include "MetricsAggregator.h"
#include "../DhNet_Client/ServerSession.h"
#include "../../DhNet_Server/DhNet_Protocol/PacketList.h"

namespace
{
    using Clock = std::chrono::steady_clock;

    void SendLobbyChat(const std::shared_ptr<Session>& _session)
    {
        auto [pkt, sender] = Sender::GetSenderAndPacket<ReqLobbyChat>();
        pkt->Init("StressTest lobby chat");
        _session->Send(sender);
    }

    void SendRoomEnter(const std::shared_ptr<Session>& _session)
    {
        auto [pkt, sender] = Sender::GetSenderAndPacket<ReqRoomEnter>();
        pkt->Init();
        _session->Send(sender);
    }

    void SendRoomChat(const std::shared_ptr<Session>& _session)
    {
        auto [pkt, sender] = Sender::GetSenderAndPacket<ReqRoomChat>();
        pkt->Init("StressTest room chat");
        _session->Send(sender);
    }

    void SendRoomExit(const std::shared_ptr<Session>& _session)
    {
        auto [pkt, sender] = Sender::GetSenderAndPacket<ReqRoomExit>();
        pkt->Init(PacketEnum::Req_RoomExit, sizeof(ReqRoomExit));
        _session->Send(sender);
    }

    void SendPing(const std::shared_ptr<Session>& _session)
    {
        auto [pkt, sender] = Sender::GetSenderAndPacket<TestPacket>();
        pkt->Init(PacketEnum::Test, sizeof(TestPacket));
        _session->Send(sender);
    }
}

bool Scenario_HandleResLobbyEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
    auto now = Clock::now();
    ScenarioStateRegistry::Instance().Apply(_session.get(), [now](ScenarioState& _state)
    {
        _state.phase = ScenarioPhase::InLobby;
        _state.phaseStartTime = now;
        _state.lastActionTime = now;
        _state.lobbyChatsSent = 0;
    });
    return true;
}

bool Scenario_HandleNotiRoomEnterPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
    auto pkt = reinterpret_cast<NotiRoomEnter*>(_header);
    auto serverSession = std::static_pointer_cast<ServerSession>(_session);
    if (pkt->m_playerId != serverSession->GetPlayerId())
        return true; // 같은 룸의 다른 플레이어 입장 알림 — 무시

    auto now = Clock::now();
    ScenarioStateRegistry::Instance().Apply(_session.get(), [now](ScenarioState& _state)
    {
        _state.phase = ScenarioPhase::InRoom;
        _state.phaseStartTime = now;
        _state.lastActionTime = now;
        _state.roomChatsSent = 0;
    });
    return true;
}

void AdvanceScenario(const std::shared_ptr<Session>& _session, Clock::time_point _now)
{
    // 레지스트리 락을 쥔 채로는 상태만 결정하고, 실제 Send()(네트워크 syscall)는
    // 락 해제 후 아래에서 수행한다 — 그렇지 않으면 매 틱마다 모든 세션의 전송이
    // 레지스트리 락 하나로 직렬화된다.
    ScenarioAction action = ScenarioAction::None;

    ScenarioStateRegistry::Instance().Apply(_session.get(), [&](ScenarioState& _state)
    {
        const auto& cfg = g_scenarioConfig;

        switch (_state.phase)
        {
        case ScenarioPhase::InLobby:
            if (_state.lobbyChatsSent < cfg.lobbyChatCount &&
                _now - _state.lastActionTime >= std::chrono::milliseconds(cfg.lobbyChatIntervalMs))
            {
                action = ScenarioAction::SendLobbyChat;
                _state.lobbyChatsSent++;
                _state.lastActionTime = _now;
            }
            else if (_state.lobbyChatsSent >= cfg.lobbyChatCount)
            {
                action = ScenarioAction::SendRoomEnter;
                _state.phase = ScenarioPhase::AwaitingRoomEnter;
                _state.phaseStartTime = _now;
                _state.actionRetries = 0;
            }
            break;

        case ScenarioPhase::AwaitingRoomEnter:
            // Noti_RoomEnter가 타임아웃 내에 안 오면 재시도 (룸이 가득 찬 race condition 등 대비).
            // 서버의 Req_RoomEnter 처리는 멱등하지 않으므로(중복 Room::Enter race 위험)
            // 재시도 횟수를 한도까지만 허용하고 초과 시 Stuck으로 전환해 더 이상 보내지 않는다.
            if (_now - _state.phaseStartTime >= std::chrono::milliseconds(cfg.roomActionTimeoutMs))
            {
                if (_state.actionRetries >= cfg.maxRoomActionRetries)
                {
                    _state.phase = ScenarioPhase::Stuck;
                    action = ScenarioAction::LogStuckRoomEnter;
                }
                else
                {
                    action = ScenarioAction::SendRoomEnter;
                    _state.actionRetries++;
                    _state.phaseStartTime = _now;
                }
            }
            break;

        case ScenarioPhase::InRoom:
            if (_state.roomChatsSent < cfg.roomChatCount &&
                _now - _state.lastActionTime >= std::chrono::milliseconds(cfg.roomChatIntervalMs))
            {
                action = ScenarioAction::SendRoomChat;
                _state.roomChatsSent++;
                _state.lastActionTime = _now;
            }
            else if (_state.roomChatsSent >= cfg.roomChatCount)
            {
                action = ScenarioAction::SendRoomExit;
                _state.phase = ScenarioPhase::AwaitingRoomExit;
                _state.phaseStartTime = _now;
                _state.actionRetries = 0;
            }
            break;

        case ScenarioPhase::AwaitingRoomExit:
            // Res_LobbyEnter가 타임아웃 내에 안 오면 재시도 (한도는 위와 동일한 이유로 적용)
            if (_now - _state.phaseStartTime >= std::chrono::milliseconds(cfg.roomActionTimeoutMs))
            {
                if (_state.actionRetries >= cfg.maxRoomActionRetries)
                {
                    _state.phase = ScenarioPhase::Stuck;
                    action = ScenarioAction::LogStuckRoomExit;
                }
                else
                {
                    action = ScenarioAction::SendRoomExit;
                    _state.actionRetries++;
                    _state.phaseStartTime = _now;
                }
            }
            break;

        case ScenarioPhase::Stuck:
        case ScenarioPhase::Handshaking:
        default:
            break; // Stuck: 더 이상 진행하지 않음. Handshaking: 로그인 완료(Res_LobbyEnter 수신) 전까지 대기.
        }
    });

    switch (action)
    {
    case ScenarioAction::SendLobbyChat: SendLobbyChat(_session); break;
    case ScenarioAction::SendRoomEnter: SendRoomEnter(_session); break;
    case ScenarioAction::SendRoomChat:  SendRoomChat(_session);  break;
    case ScenarioAction::SendRoomExit:  SendRoomExit(_session);  break;
    case ScenarioAction::LogStuckRoomEnter:
        std::wcerr << L"[부하테스트] RoomEnter 재시도 한도 초과 — 세션이 Stuck 상태로 전환됨 "
                       L"(서버 응답 누락 또는 race 의심)" << std::endl;
        break;
    case ScenarioAction::LogStuckRoomExit:
        std::wcerr << L"[부하테스트] RoomExit 재시도 한도 초과 — 세션이 Stuck 상태로 전환됨 "
                       L"(서버 응답 누락 또는 race 의심)" << std::endl;
        break;
    case ScenarioAction::None:
    default:
        break;
    }
}

bool Scenario_HandleResTestPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
    // AdvanceScenario와 동일한 원칙: 레지스트리 락 안에서는 상태만 읽고 결정하고,
    // 다른 객체(MetricsAggregator)의 락은 레지스트리 락 해제 후에 잡는다.
    auto now = Clock::now();
    bool shouldRecord = false;
    double rttMs = 0.0;

    ScenarioStateRegistry::Instance().Apply(_session.get(), [now, &shouldRecord, &rttMs](ScenarioState& _state)
    {
        if (!_state.pingInFlight)
            return; // 타임아웃 처리로 이미 종료된 핑의 늦은 응답 — 무시

        rttMs = std::chrono::duration<double, std::milli>(now - _state.pingSentTime).count();
        shouldRecord = true;
        _state.pingInFlight = false;
    });

    if (shouldRecord)
        MetricsAggregator::Instance().RecordRtt(rttMs);

    return true;
}

void AdvancePing(const std::shared_ptr<Session>& _session, Clock::time_point _now)
{
    bool shouldSendPing = false;
    bool shouldRecordTimeout = false;

    ScenarioStateRegistry::Instance().Apply(_session.get(), [&](ScenarioState& _state)
    {
        const auto& cfg = g_scenarioConfig;

        if (_state.pingInFlight &&
            _now - _state.pingSentTime >= std::chrono::milliseconds(cfg.pingTimeoutMs))
        {
            shouldRecordTimeout = true;
            _state.pingInFlight = false; // 다음 인터벌에 자연스럽게 재시도됨
        }

        if (!_state.pingInFlight &&
            _now - _state.lastPingSentTime >= std::chrono::milliseconds(cfg.pingIntervalMs))
        {
            shouldSendPing = true;
            _state.pingInFlight = true;
            _state.pingSentTime = _now;
            _state.lastPingSentTime = _now;
        }
    });

    if (shouldRecordTimeout)
        MetricsAggregator::Instance().RecordTimeout();
    if (shouldSendPing)
        SendPing(_session);
}
