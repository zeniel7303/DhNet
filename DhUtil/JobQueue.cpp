#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(JobRef&& _job)
{
	// ���� �߿� - count ���� �� push, job ���� �� count ����
	const int32 prevCount = m_jobCount.fetch_add(1);
	m_jobs.Push(_job); // WRITE_LOCK

	// ù��° Job�� ���� �����尡 ������� ���
	if (prevCount == 0)
	{
	    if (LCurrentJobQueue != nullptr)
	    {
	    	// �̹� ���� ���� ���� JobQueue�� �����Ƿ� ���� �ִ� �ٸ� �����忡�� �۾� ����
	    	GGlobalQueue->Push(shared_from_this());
			return;    
	    }

		// �̹� ���� ���� ���� JobQueue�� �����Ƿ� ���� ����
		Execute();
	}
}

// TODO : 
// 1. �ϰ��� �ѹ���, �׸��� ��� �ʹ� ������ ��Ȳ
//		-> Execute���� �ð� üũ�� ���� �ذ�
// 2. DoAsync �Լ��� ��� Ÿ��(DoAsync�ؼ� ������ �Լ� �ȿ� �� DoAsync�� �ִ� ��...) 
//    ���� ������ �ʴ� ��Ȳ (�ϰ��� �� ���������� ������ ��Ȳ)
//		-> GGlobalQueue �л� ó���� �ذ�
void JobQueue::Execute()
{
	LCurrentJobQueue = this;
	
    while (true)
    {
        std::vector<JobRef> jobs;
        m_jobs.PopAll(OUT jobs);

        const int32 jobCount = static_cast<int32>(jobs.size());
        for (int32 i = 0; i < jobCount; i++)
        {
            try
            {
                jobs[i]->Execute();
            }
            catch (const std::exception& e)
            {
                // �۾� ���� �� ���� ó��
                std::cerr << "Job execution failed: " << e.what() << std::endl;
            }
        }

        // �ϰ��� �� �̻� ������ �ߴ�
        if (m_jobCount.fetch_sub(jobCount) == jobCount)
        {
        	LCurrentJobQueue = nullptr;
            return;
        }

    	const uint64 now = ::GetTickCount64();
    	if (now >= LEndTickCount)
    	{
    		// �����ϰ� �� �����尡 �۾��� ���� �����ϰ� ������ �ߴ�
    		LCurrentJobQueue = nullptr;
    		// ���� �ִ� �ٸ� �����尡 �����ϵ��� GlobalQueue�� �ѱ��
    		GGlobalQueue->Push(shared_from_this());
    		break;
    	}
    }
}