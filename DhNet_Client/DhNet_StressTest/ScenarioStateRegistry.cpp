#include "stdafx.h"
#include "ScenarioStateRegistry.h"

ScenarioStateRegistry& ScenarioStateRegistry::Instance()
{
    static ScenarioStateRegistry instance;
    return instance;
}

void ScenarioStateRegistry::Register(Session* _session)
{
    WRITE_LOCK;
    m_states[_session] = ScenarioState{};
}

void ScenarioStateRegistry::Unregister(Session* _session)
{
    WRITE_LOCK;
    m_states.erase(_session);
}

void ScenarioStateRegistry::Apply(Session* _session, const std::function<void(ScenarioState&)>& _func)
{
    WRITE_LOCK;
    auto it = m_states.find(_session);
    if (it == m_states.end())
        return;

    _func(it->second);
}
