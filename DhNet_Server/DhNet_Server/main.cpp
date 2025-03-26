#include "stdafx.h"

#include "../../DhUtil/ThreadManager.h"
#include "../ServerCore/Service.h"

#include "ServerPacketHandler.h"
#include "GameSession.h"

ThreadManager* GThreadManager = new ThreadManager();

void PacketRegister()
{
    PacketHandler::Instance().Register(PacketEnum::Test, &HandleTestPacket);
}

void StartServer()
{
    shared_ptr<ServerService> serverService = make_shared<ServerService>(
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
                    // �ڵ����� Listener�� Dispatch�� ����.(�����Լ��̹Ƿ�)
                    serverService->GetIocpCore()->Dispatch();
                }
            });
    }

    GThreadManager->Join();
}

int main()
{
    PacketRegister();

    StartServer();
}