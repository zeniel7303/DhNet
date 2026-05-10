#pragma once
#include "stdafx.h"

#include "../../DhUtil/ThreadManager.h"
#include "../../DhNet_Server/ServerCore/Service.h"

#include "ServerSession.h"

#include "TestController.h"
#include "LoginController.h"
#include "ClientLobbyController.h"
#include "RoomController.h"

ThreadManager* GThreadManager = new ThreadManager();

int main()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));

	Sender::Init(1024);

	PacketHandler::Instance().Register(PacketEnum::Test, &RecvTestPacket);
	PacketHandler::Instance().Register(PacketEnum::Res_Login, &HandleResLoginPacket);
	PacketHandler::Instance().Register(PacketEnum::Res_LoginFailed, &HandleResLoginFailedPacket);
	PacketHandler::Instance().Register(PacketEnum::Res_LobbyEnter, &HandleResLobbyEnterPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_LobbyPlayerEnter, &HandleNotiLobbyPlayerEnterPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_LobbyPlayerExit, &HandleNotiLobbyPlayerExitPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_LobbyChat, &HandleNotiLobbyChatPacket);
	PacketHandler::Instance().Register(PacketEnum::Res_RoomEnter, &HandleResRoomEnterPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_RoomEnter, &HandleNotiRoomEnterPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_RoomChat, &HandleNotiRoomChatPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_RoomExit, &HandleNotiRoomExitPacket);

	ClientServiceRef clientService = std::make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 7900),
		std::make_shared<IocpCore>(),
		[]() { return std::make_shared<ServerSession>(); },
		1);

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

	while (1)
	{
		if (g_inRoom.load())
		{
			auto senderAndPacket = Sender::GetSenderAndPacket<ReqRoomChat>();
			senderAndPacket.first->Init("Test Message");
			clientService->BroadCast(senderAndPacket.second);
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
