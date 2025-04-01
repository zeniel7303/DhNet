#pragma once

class Job
{
public:
	virtual void Execute() {}
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