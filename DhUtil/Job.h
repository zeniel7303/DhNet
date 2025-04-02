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
	tuple<Args...> m_tuple;

public:
	FuncJob(FuncType _func, Args... _args) : m_func(_func), m_tuple(_args...)
	{

	}

	virtual void Execute() override
	{
		apply([this](auto&&... args) {
			invoke(m_func, forward<decltype(args)>(args)...);
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
	tuple<Args...> m_tuple;

public:
	MemberJob(T* _obj, FuncType _func, Args... _args) : m_obj(_obj), m_func(_func), m_tuple(_args...)
	{

	}

	virtual void Execute() override
	{
		apply([this](auto&&... args) {
			invoke(m_func, m_obj, forward<decltype(args)>(args)...);
			}, m_tuple);
	}
};

class JobQueue
{
private:
	USE_LOCK;
	queue<shared_ptr<Job>> m_jobs;

public:
	void Push(shared_ptr<Job> _job)
	{
		WRITE_LOCK;
		m_jobs.push(_job);
	}

	shared_ptr<Job> Pop()
	{
		WRITE_LOCK;
		if (m_jobs.empty())
			return nullptr;

		shared_ptr<Job> ret = m_jobs.front();
		m_jobs.pop();
		return ret;
	}
};