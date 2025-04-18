#include "pch.h"
#include "TLS.h"

thread_local unsigned __int32	LThreadId = 0;
thread_local uint64				LEndTickCount = 0;

thread_local std::stack<int32>	LLockStack;
thread_local JobQueue*			LCurrentJobQueue = nullptr;