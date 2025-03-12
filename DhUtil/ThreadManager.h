#pragma once
#include <mutex>
#include <vector>
#include <thread>
#include <functional>

/*--------------
  ThreadManager
---------------*/

class ThreadManager
{
private:
	mutex			m_lock;
	vector<thread>	m_threads;

public:
	ThreadManager();
	~ThreadManager();

	void	Launch(function<void(void)> _callback);
	void	Join();

	static void InitTLS();
	static void DestroyTLS();
};