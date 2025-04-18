#include "pch.h"
#include "ThreadManager.h"
#include "TLS.h"

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