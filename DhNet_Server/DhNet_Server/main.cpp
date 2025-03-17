#include "stdafx.h"
#include "../../DhUtil/ThreadManager.h"
#include "../ServerCore/Service.h"

ThreadManager* GThreadManager = new ThreadManager();

class GameSession : public Session
{
public:
	~GameSession()
	{
		cout << "~GameSession()" << endl;
	}

	virtual int32 OnRecv(BYTE* _buffer, int32 _len) override
	{
		cout << "OnRecv Len" << _len << endl;
		Send(_buffer, _len);
		return _len;
	}

	virtual void OnSend(int32 _len) override
	{
		cout << "OnSend Len" << _len << endl;
	}
};

int main()
{
	shared_ptr<ServerService> serverService =  make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[]() { return make_shared<GameSession>(); },
		1000);

	ASSERT_CRASH(serverService->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					// 자동으로 Listener의 Dispatch도 들어간다.(가상함수이므로)
					serverService->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}