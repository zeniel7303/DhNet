#pragma once

class Job
{
public:
	virtual void Execute() {}
};

template<typename Ret, typename... Args>
class FuncJob : public Job
{
	using FuncType = Ret(*)(Args...);

private:
	FuncType m_func;
	std::tuple<Args...> m_tuple;

public:
	FuncJob(FuncType _func, Args... _args) : m_func(_func), m_tuple(_args...)
	{

	}

	virtual void Execute() override
	{
		apply([this](auto&&... args) {
			std::invoke(m_func, std::forward<decltype(args)>(args)...);
			}, m_tuple);
	}
};

template<typename T, typename Ret, typename... Args>
class MemberJob : public Job
{
	using FuncType = Ret(T::*)(Args...);

private:
	T* m_obj;
	FuncType	m_func;
	std::tuple<Args...> m_tuple;

public:
	MemberJob(T* _obj, FuncType _func, Args... _args) : m_obj(_obj), m_func(_func), m_tuple(_args...)
	{

	}

	virtual void Execute() override
	{
		apply([this](auto&&... args) {
			std::invoke(m_func, m_obj, std::forward<decltype(args)>(args)...);
			}, m_tuple);
	}
};

class JobQueue
{
private:
	USE_LOCK;
	std::queue<JobRef> m_jobs;

public:
	void Push(JobRef _job)
	{
		WRITE_LOCK;
		m_jobs.push(_job);
	}

	JobRef Pop()
	{
		WRITE_LOCK;
		if (m_jobs.empty())
			return nullptr;

		JobRef ret = m_jobs.front();
		m_jobs.pop();
		return ret;
	}
};