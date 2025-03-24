#include "stdafx.h"
#include "../../DhUtil/ThreadManager.h"

#include "../../DhNet_Server/ServerCore/Service.h"
#include "../../DhNet_Server/ServerCore/Session.h"

#include "TempPacketList.h"

char sendData[] = "Hello World";

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

		auto sender = Sender::Alloc<TestPacket>();
		auto packet = sender->GetWritePointer<TestPacket>();
		packet->m_packetNum = 0;
		strcpy_s(packet->m_test, "Connect");
		Send(sender);
	}

	virtual bool OnRecv(PacketHeader* _packet) override
	{
		auto temp = reinterpret_cast<TestPacket*>(_packet);

		// Echo
		cout << "OnRecv Len = " << _packet->m_dataSize << " / " << temp->m_test << endl;

		this_thread::sleep_for(1s);

		auto sender = Sender::Alloc<TestPacket>();
		auto packet = sender->GetWritePointer<TestPacket>();
		packet->m_packetNum = 0;
		strcpy_s(packet->m_test, "Test(Client)");
		Send(sender);

		return true;
	}

	virtual void OnSend(int32 _len) override
	{
		// cout << "OnSend Len = " << _len << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}
};

int main()
{
	this_thread::sleep_for(2s);

	Sender::Init(1024);

	shared_ptr<ClientService> clientService = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[]() { return make_shared<ServerSession>(); },
		10);

	ASSERT_CRASH(clientService->Start());

	for (int32 i = 0; i < 10; i++)
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