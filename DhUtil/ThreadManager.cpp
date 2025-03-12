#include "pch.h"
#include "ThreadManager.h"
#include "TLS.h"

/*--------------
  ThreadManager
---------------*/

ThreadManager::ThreadManager()
{
	// Main Thread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> _callback)
{
	lock_guard<std::mutex> guard(m_lock);

	m_threads.push_back(thread([=]()
		{
			InitTLS();
			_callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	for (thread& t : m_threads)
	{
		if (t.joinable())
			t.join();
	}
	m_threads.clear();
}

void ThreadManager::InitTLS()
{
	static atomic<unsigned __int32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{

}