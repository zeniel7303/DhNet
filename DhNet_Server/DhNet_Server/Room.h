#pragma once
#include "GameSession.h"
#include "Player.h"
#include "../../DhUtil/Job.h"

class Room
{
private:
	std::map<uint64, std::shared_ptr<Player>> m_players;

	JobQueue m_jobs;

public:
	void Enter(std::shared_ptr<Player> _player);
	void Leave(std::shared_ptr<Player> _player);
	void Broadcast(std::shared_ptr<Sender> _sender);
	void FlushJob();

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::* _memFunc)(Args...), Args... _args)
	{
		auto job = std::make_shared<MemberJob<T, Ret, Args...>>(static_cast<T*>(this), _memFunc, _args...);
		m_jobs.Push(job);
	}
};

extern Room GRoom;