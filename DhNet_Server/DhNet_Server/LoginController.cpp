#include "stdafx.h"
#include "LoginController.h"
#include "GameSession.h"
#include "GameServer.h"
#include "DbSystem.h"
#include "DbConnection.h"
#include "AccountRepository.h"
#include "../../DhUtil/ThreadManager.h"

bool HandleReqLoginPacket(PacketHeader* _header, std::shared_ptr<Session>& _session)
{
    auto reqLogin    = reinterpret_cast<ReqLogin*>(_header);
    auto gameSession = std::static_pointer_cast<GameSession>(_session);

    char username[17] = {};
    char password[65] = {};
    strncpy_s(username, reqLogin->m_username,    sizeof(reqLogin->m_username));
    strncpy_s(password, reqLogin->m_password, sizeof(reqLogin->m_password));

    std::string usernameStr(username);
    std::string passwordStr(password);

    size_t uLen = usernameStr.size();
    size_t pLen = passwordStr.size();
    if (uLen < 4 || uLen > 16 || pLen < 8 || pLen > 64)
    {
        auto sp = Sender::GetSenderAndPacket<ResLoginFailed>();
        sp.first->Init();
        _session->Send(sp.second);
        return true;
    }

    DbSystem* dbSystem = GameServer::Instance().GetSystem<DbSystem>();
    if (!dbSystem)
    {
        auto sp = Sender::GetSenderAndPacket<ResLoginFailed>();
        sp.first->Init();
        _session->Send(sp.second);
        return true;
    }

    dbSystem->Execute([gameSession, usernameStr, passwordStr, dbSystem]()
    {
        DbConnection conn(dbSystem->GetPool());

        uint64      accountId  = 0;
        std::string playerName;
        bool ok = conn.IsValid() && AccountRepository::Authenticate(
            conn.Get(), usernameStr, passwordStr, accountId, playerName);

        ThreadManager::PushGlobalQueue([gameSession, ok, accountId, playerName]()
        {
            gameSession->OnLoginResult(ok, accountId, playerName);
        });
    });

    return true;
}