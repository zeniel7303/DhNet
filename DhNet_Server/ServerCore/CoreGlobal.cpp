#include "pch.h"
#include "CoreGlobal.h"
#include "SocketUtils.h"

class CoreGlobal
{
public:
	CoreGlobal()
	{
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		SocketUtils::Clear();
	}
} GCoreGlobal;