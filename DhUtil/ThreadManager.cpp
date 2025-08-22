#include "pch.h"
#include "ThreadManager.h"
#include "TLS.h"
#include "GlobalQueue.h"
#include "JobQueue.h"

ThreadManager::ThreadManager()
{
	// Main Thread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(std::function<void(void)> _callback)
{
	std::lock_guard<std::mutex> guard(m_lock);

	m_threads.push_back(std::thread([=]()
		{
			InitTLS();
			_callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	for (std::thread& t : m_threads)
	{
		if (t.joinable())
			t.join();
	}
	m_threads.clear();
}

void ThreadManager::InitTLS()
{
	static std::atomic<unsigned __int32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{

}

void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		if (::GetTickCount64() > LEndTickCount)
			break;

		JobQueueRef jobQueue = GGlobalQueue->Pop();
		if (jobQueue == nullptr)
			break;

		jobQueue->Execute();
	}
}