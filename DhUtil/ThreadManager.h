#pragma once
#include <mutex>
#include <vector>
#include <thread>
#include <functional>

class ThreadManager
{
private:
	std::mutex					m_lock;
	std::vector<std::thread>	m_threads;

public:
	ThreadManager();
	~ThreadManager();

	void Launch(std::function<void(void)> _callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

	static void DoGlobalQueueWork();
};