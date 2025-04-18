#pragma once
#include "LockQueue.h"

class GlobalQueue
{
private:
	LockQueue<JobQueueRef> m_jobQueues;

public:
	GlobalQueue();
	~GlobalQueue();

	void					Push(JobQueueRef _jobQueue);
	JobQueueRef				Pop();
};