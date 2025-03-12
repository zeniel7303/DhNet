#include "pch.h"
#include "CoreGlobal.h"
#include "SocketUtils.h"

#include "IocpCore.h"

// Temp
IocpCore* GIocpCore = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GIocpCore = new IocpCore();

		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GIocpCore;

		SocketUtils::Clear();
	}
} GCoreGlobal;