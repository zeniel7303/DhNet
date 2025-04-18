#pragma once
#include <stack>

extern thread_local unsigned __int32	LThreadId;
extern thread_local uint64				LEndTickCount;

extern thread_local std::stack<int32>	LLockStack;
extern thread_local class JobQueue*		LCurrentJobQueue;