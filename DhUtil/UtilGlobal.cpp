#include "pch.h"
#include "UtilGlobal.h"
#include "DeadLockProfiler.h"

DeadLockProfiler* GDeadLockProfiler = nullptr;

class UtilGlobal
{
public:
	UtilGlobal()
	{
		GDeadLockProfiler = new DeadLockProfiler();
	}

	~UtilGlobal()
	{
		delete GDeadLockProfiler;
	}
} GUtilGlobal;