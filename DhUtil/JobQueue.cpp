#include "pch.h"
#include "JobQueue.h"

void JobQueue::Push(JobRef&& _job)
{
	// ���� �߿� - count ���� �� push, job ���� �� count ����
	const int32 prevCount = m_jobCount.fetch_add(1);
	m_jobs.Push(_job); // WRITE_LOCK

	// ù��° Job�� ���� �����尡 ������� ���
	if (prevCount == 0)
	{
		Execute();
	}
}

// TODO : 
// 1. �ϰ��� �ѹ���, �׸��� ��� �ʹ� ������ ��Ȳ
// 2. DoAsync �Լ��� ��� Ÿ��(DoAsync�ؼ� ������ �Լ� �ȿ� �� DoAsync�� �ִ� ��...) 
//    ���� ������ �ʴ� ��Ȳ (�ϰ��� �� ���������� ������ ��Ȳ)
void JobQueue::Execute()
{
	while (true)
	{
		std::vector<JobRef> jobs;
		m_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		// ���� �ϰ��� 0����� ����
		if (m_jobCount.fetch_sub(jobCount) == jobCount)
		{
			return;
		}
	}
}
