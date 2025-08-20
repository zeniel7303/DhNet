#include "pch.h"
#include "Service.h"
#include "SocketUtils.h"

Service::Service(NetAddress _address, IocpCoreRef _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount)
	: m_netAddress(_address), m_iocpCore(_iocpCore), m_sessionFactory(_sessionFactory), m_maxSessionCount(_maxSessionCount)
{
	SocketUtils::Init();
}

Service::~Service()
{
	SocketUtils::Clear();
}

SessionRef Service::CreateSession()
{
    SessionRef session = m_sessionFactory();
	session->SetService(shared_from_this());

	if (m_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(SessionRef _session)
{
	WRITE_LOCK;

	m_sessionCount++;
	m_sessions.insert(_session);
}

void Service::ReleaseSession(SessionRef _session)
{
	WRITE_LOCK;

	ASSERT_CRASH(m_sessions.erase(_session) != 0);
	m_sessionCount--;
}

void Service::BroadCast(SenderRef _sender)
{
	WRITE_LOCK;
	for (const auto& session : m_sessions)
	{
		session->Send(_sender);
	}
}

ServerService::ServerService(NetAddress _netAddress, IocpCoreRef _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount)
	: Service(_netAddress, _iocpCore, _sessionFactory, _maxSessionCount)
{
	m_isServerService = true;
}

ServerService::~ServerService()
{
}

bool ServerService::Start()
{
	m_listener = std::make_shared<Listener>();
	if (m_listener == nullptr)
		return false;

    if (m_listener->StartAccept(std::static_pointer_cast<ServerService>(shared_from_this())) == false)
		return false;

	Sender::Init(30000);

	std::cout << "Server Start!" << std::endl;

	return true;
}

void ServerService::End()
{
}

ClientService::ClientService(NetAddress _netAddress, IocpCoreRef _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount)
	: Service(_netAddress, _iocpCore, _sessionFactory, _maxSessionCount)
{
	m_isServerService = false;
}

ClientService::~ClientService()
{
}

bool ClientService::Start()
{
	for (int32 i = 0; i < m_maxSessionCount; i++)
	{
		SessionRef session = CreateSession();
		if (session->Connect() == false)
			return false;
	}

	return true;
}

void ClientService::End()
{
	// Gracefully disconnect all client sessions
	std::vector<SessionRef> sessions;
	{
		WRITE_LOCK;
		for (auto& s : m_sessions)
			sessions.push_back(s);
	}
	for (auto& s : sessions)
	{
		if (s && s->IsConnected())
			s->Disconnect(L"ClientService::End");
	}
}