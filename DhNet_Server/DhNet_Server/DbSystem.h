#pragma once
#include "DbConnectionPool.h"

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

class ServerSetting;

class DbSystem
{
	DbConnectionPool					_pool;
	std::vector<std::thread>			_threads;
	std::queue<std::function<void()>>	_tasks;
	std::mutex							_mutex;
	std::condition_variable				_cv;
	std::atomic<bool>					_running{ false };

public:
	~DbSystem();

	bool	Init(const ServerSetting& setting);
	void	Shutdown();
	void	Execute(std::function<void()> task);

	DbConnectionPool* GetPool() { return &_pool; }

private:
	void WorkerThread();
};
