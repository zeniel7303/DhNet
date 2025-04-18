#pragma once
#include "ObjectPool.h"
#include "LockQueue.h"
#include "Job.h"

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
private:
	LockQueue<JobRef>		m_jobs;
	std::atomic<int32>		m_jobCount;

private:
	void Push(JobRef&& _job);
	void Execute();

public:
	void DoAsync(CallbackType&& _callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(_callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* _memFunc)(Args...), Args... _args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, _memFunc, std::forward<Args>(_args)...));
	}

	void ClearJobs() { m_jobs.Clear(); }
};