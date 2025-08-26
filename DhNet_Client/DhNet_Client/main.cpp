#pragma once
#include "stdafx.h"

#include "../../DhUtil/ThreadManager.h"
#include "../../DhNet_Server/ServerCore/Service.h"

#include "ServerSession.h"

#include "TestController.h"
#include "LoginController.h"
#include "RoomController.h"

ThreadManager* GThreadManager = new ThreadManager();

int main()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));

	Sender::Init(1024);

	PacketHandler::Instance().Register(PacketEnum::Test, &RecvTestPacket);
	PacketHandler::Instance().Register(PacketEnum::Res_Login, &HandleResLoginPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_RoomEnter, &HandleNotiRoomEnterPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_RoomChat, &HandleNotiRoomChatPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_RoomExit, &HandleNotiRoomExitPacket);

	ClientServiceRef clientService = std::make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		std::make_shared<IocpCore>(),
		[]() { return std::make_shared<ServerSession>(); },
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

	auto senderAndPacket = Sender::GetSenderAndPacket<ReqRoomChat>();
	senderAndPacket.first->Init(PacketEnum::Req_RoomChat, sizeof(ReqRoomChat));
    strcpy_s(senderAndPacket.first->m_message, "Test Message");

	while (1)
	{
		clientService->BroadCast(senderAndPacket.second);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	GThreadManager->Join();
}