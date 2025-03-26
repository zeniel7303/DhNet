#include "stdafx.h"

#include "../../DhUtil/ThreadManager.h"
#include "../../DhNet_Server/ServerCore/Service.h"
#include "../../DhNet_Server/ServerCore/PacketHandler.h"

#include "ClientPacketHandler.h"
#include "ServerSession.h"

ThreadManager* GThreadManager = new ThreadManager();

int main()
{
	this_thread::sleep_for(2s);

	Sender::Init(1024);

	PacketHandler::Instance().Register(PacketEnum::Test, &RecvTestPacket);

	shared_ptr<ClientService> clientService = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[]() { return make_shared<ServerSession>(); },
		2);

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