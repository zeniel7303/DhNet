#pragma once

#include "Logger.h"

#define OUT

/*----------------------------
			 Log
------------------------------*/

#define LOG_TRACE(...)		GLogger->trace(__VA_ARGS__)
#define LOG_DEBUG(...)		GLogger->debug(__VA_ARGS__)
#define LOG_INFO(...)		GLogger->info(__VA_ARGS__)
#define LOG_WARN(...)		GLogger->warn(__VA_ARGS__)
#define LOG_ERROR(...)		GLogger->error(__VA_ARGS__)
#define LOG_CRITICAL(...)	GLogger->critical(__VA_ARGS__)

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

#define CRASH(cause)					\
{										\
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