#include "stdafx.h"
#include "../ServerCore/Listener.h"
#include "../../DhUtil/ThreadManager.h"

ThreadManager* GThreadManager = new ThreadManager();

int main()
{
	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));

	for (__int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					// 자동으로 Listener의 Dispatch도 들어간다.(가상함수이므로)
					GIocpCore->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}