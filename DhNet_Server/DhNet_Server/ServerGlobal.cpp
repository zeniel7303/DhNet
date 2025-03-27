#include "stdafx.h"
#include "ServerGlobal.h"
#include "../../DhUtil/ThreadManager.h"

ThreadManager* GThreadManager = nullptr;

class ServerGlobal
{
public:
	ServerGlobal()
	{
		GThreadManager = new ThreadManager();
	}
	~ServerGlobal()
	{
		delete GThreadManager;
	}
} GServerGlobal;