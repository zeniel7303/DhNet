#pragma once

#include <thread>
#include <chrono>
#include "Logger.h"

#define OUT

/*----------------------------
			 Log
------------------------------*/

#define LOG_TRACE(...)		Logger::Get()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)		Logger::Get()->debug(__VA_ARGS__)
#define LOG_INFO(...)		Logger::Get()->info(__VA_ARGS__)
#define LOG_WARN(...)		Logger::Get()->warn(__VA_ARGS__)
#define LOG_ERROR(...)		Logger::Get()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)	Logger::Get()->critical(__VA_ARGS__)

/*----------------------------
			 Lock
------------------------------*/

#define USE_MANY_LOCKS(count)	Lock m_locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(m_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(m_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

/*----------------------------
			Crash
------------------------------*/

// LOG_CRITICAL/LoggerShutdown()이 어디서든(enqueue, flush 등) 멈추더라도
// 워치독 스레드가 500ms 후 무조건 크래시시켜 fail-fast를 보장한다.
#define CRASH(cause)					\
{										\
	try {								\
		std::thread([] {				\
			std::this_thread::sleep_for(std::chrono::milliseconds(500)); \
			unsigned __int32* crash = nullptr;	\
			__analysis_assume(crash != nullptr);\
			*crash = 0xDEADBEEF;		\
		}).detach();					\
	} catch (...) {}					\
	LOG_CRITICAL("CRASH: {} ({}:{})", cause, __FILE__, __LINE__); \
	LoggerShutdown();					\
	unsigned __int32* crash = nullptr;	\
	__analysis_assume(crash != nullptr);\
	*crash = 0xDEADBEEF;				\
}

#define ASSERT_CRASH(expr)				\
{										\
	if(!(expr))							\
	{									\
		CRASH("ASSERT_CRASH");			\
		__analysis_assume(expr);		\
	}									\
}