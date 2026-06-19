#pragma once
#include <unordered_map>
#include <functional>

#include "../../DhNet_Server/ServerCore/Session.h"
#include "ScenarioState.h"

// Session*를 키로 시나리오 상태를 보관한다. 세션 연결/해제 시점에 Register/Unregister가
// 호출된다는 전제 하에 안전함 — Unregister 누락 시 포인터 재사용으로 stale 상태가
// 새 세션에 잘못 적용될 수 있으므로 반드시 OnDisconnected 경로에서 호출해야 한다.
class ScenarioStateRegistry
{
    USE_LOCK;
    std::unordered_map<Session*, ScenarioState> m_states;

public:
    static ScenarioStateRegistry& Instance();

    void Register(Session* _session);
    void Unregister(Session* _session);

    // _session이 등록되어 있으면 _func(state)를 실행한다.
    void Apply(Session* _session, const std::function<void(ScenarioState&)>& _func);
};
