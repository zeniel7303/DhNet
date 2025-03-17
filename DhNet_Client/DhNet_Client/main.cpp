#include "stdafx.h"
#include "../../DhUtil/ThreadManager.h"

#include "../../DhNet_Server/ServerCore/Service.h"
#include "../../DhNet_Server/ServerCore/Session.h"

char sendbuffer[] = "Hello World";

ThreadManager* GThreadManager = new ThreadManager();

class ServerSession : public Session
{
public:
	~ServerSession()
	{
		cout << "~ServerSession()" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connected To Server" << endl;
		Send((BYTE*)sendbuffer, sizeof(sendbuffer));
	}

	virtual int32 OnRecv(BYTE* _buffer, int32 _len) override
	{
		// Echo
		cout << "OnRecv Len = " << _len << endl;

		this_thread::sleep_for(1s);

		//Send(_buffer, _len);
		Send((BYTE*)sendbuffer, sizeof(sendbuffer));
		return _len;
	}

	virtual void OnSend(int32 _len) override
	{
		cout << "OnSend Len = " << _len << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}
};

int main()
{
	this_thread::sleep_for(2s);

	shared_ptr<ClientService> clientService = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[]() { return make_shared<ServerSession>(); },
		1);

	ASSERT_CRASH(clientService->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					clientService->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}