#include "stdafx.h"
#include "DbSystem.h"
#include "ServerSetting.h"
#include "../../DhUtil/ThreadManager.h"

DbSystem::~DbSystem()
{
    if (_running.load())
        Shutdown();
}

bool DbSystem::Init(const ServerSetting& setting)
{
    bool poolReady = _pool.Init(
        setting.GetDbHost(), setting.GetDbPort(),
        setting.GetDbUser(), setting.GetDbPassword(), setting.GetDbName(),
        setting.GetDbPoolSize());

    if (!poolReady)
        std::cout << "[DbSystem] Warning: starting without DB — login will be rejected\n";

    // Thread count matches actual pool size so every thread has a connection
    int32 threadCount = _pool.IsReady() ? setting.GetDbPoolSize() : 0;
    _running = true;
    for (int32 i = 0; i < threadCount; i++)
        _threads.emplace_back(&DbSystem::WorkerThread, this);

    std::cout << "[DbSystem] Started " << threadCount << " worker threads\n";
    return poolReady;
}

void DbSystem::Shutdown()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _running = false;
    }
    _cv.notify_all();

    for (auto& t : _threads)
        if (t.joinable()) t.join();

    _threads.clear();
    _pool.Shutdown();
}

void DbSystem::Execute(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_running.load())
            return;
        _tasks.push(std::move(task));
    }
    _cv.notify_one();
}

void DbSystem::WorkerThread()
{
    ThreadManager::InitTLS();

    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock, [this] { return !_running.load() || !_tasks.empty(); });

            if (!_running.load() && _tasks.empty())
                break;

            task = std::move(_tasks.front());
            _tasks.pop();
        }
        task();
    }
}
