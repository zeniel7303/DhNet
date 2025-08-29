#pragma once
#include "stdafx.h"

#include "../../DhUtil/ThreadManager.h"
#include "../../DhNet_Server/ServerCore/Service.h"

#include "ServerSession.h"

#include "TestController.h"
#include "LoginController.h"
#include "LobbyController.h"
#include "RoomController.h"

ThreadManager* GThreadManager = new ThreadManager();

template<typename TPair>
void Do(ClientServiceRef _clientService, const TPair& _senderAndPacket)
{
	_clientService->BroadCast(_senderAndPacket.second);
}

void RegisterPacket()
{
	PacketHandler::Instance().Register(PacketEnum::Test, &RecvTestPacket);
	PacketHandler::Instance().Register(PacketEnum::Res_Login, &HandleResLoginPacket);
	
	PacketHandler::Instance().Register(PacketEnum::Res_RoomEnter, &HandleResRoomEnterPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_RoomEnter, &HandleNotiRoomEnterPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_RoomChat, &HandleNotiRoomChatPacket);
	PacketHandler::Instance().Register(PacketEnum::Res_RoomExit, &HandleResRoomExitPacket);
	PacketHandler::Instance().Register(PacketEnum::Noti_RoomExit, &HandleNotiRoomExitPacket);
	
	PacketHandler::Instance().Register(PacketEnum::Noti_LobbyChat, &HandleNotiLobbyChatPacket);
}

int main()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));

	Sender::Init(50000);

	RegisterPacket();
	
	ClientServiceRef clientService = std::make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		std::make_shared<IocpCore>(),
		[]() { return std::make_shared<ServerSession>(); },
		100);

	ASSERT_CRASH(clientService->Start());

	/*auto senderAndPacket = Sender::GetSenderAndPacket<ReqRoomChat>();
	senderAndPacket.first->Init(PacketEnum::Req_RoomChat, sizeof(ReqRoomChat));
	strcpy_s(senderAndPacket.first->m_message, "Test Message");*/

	auto senderAndPacket = Sender::GetSenderAndPacket<ReqLobbyChat>();
	senderAndPacket.first->Init(PacketEnum::Req_LobbyChat, sizeof(ReqLobbyChat));
	strcpy_s(senderAndPacket.first->m_message, "Test Message");

	for (int32 i = 0; i < 10; i++)
	{
		GThreadManager->Launch([=]()
			{
				auto nextBroadcast = std::chrono::steady_clock::now();

				while (true)
				{
					clientService->GetIocpCore()->Dispatch(10);

					auto now = std::chrono::steady_clock::now();
					if (now >= nextBroadcast)
					{
						Do(clientService, senderAndPacket);
						nextBroadcast = now + std::chrono::seconds(10);
					}

				}
			});
	}

	GThreadManager->Join();
}