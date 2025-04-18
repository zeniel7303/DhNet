#include "pch.h"
#include "UtilGlobal.h"
#include "DeadLockProfiler.h"
#include "GlobalQueue.h"

DeadLockProfiler* GDeadLockProfiler = nullptr;
GlobalQueue* GGlobalQueue = nullptr;

class UtilGlobal
{
public:
	UtilGlobal()
	{
		GDeadLockProfiler = new DeadLockProfiler();
		GGlobalQueue = new GlobalQueue();
	}

	~UtilGlobal()
	{
		delete GDeadLockProfiler;
		delete GGlobalQueue;
	}
} GUtilGlobal;