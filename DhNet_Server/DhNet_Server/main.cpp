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
					// �ڵ����� Listener�� Dispatch�� ����.(�����Լ��̹Ƿ�)
					GIocpCore->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}