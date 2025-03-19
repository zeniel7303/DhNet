#include "pch.h"
#include "Service.h"
#include "SocketUtils.h"

Service::Service(NetAddress _address, shared_ptr<IocpCore> _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount)
	: m_netAddress(_address), m_iocpCore(_iocpCore), m_sessionFactory(_sessionFactory), m_maxSessionCount(_maxSessionCount)
{
	SocketUtils::Init();
}

Service::~Service()
{
	SocketUtils::Clear();
}

shared_ptr<Session> Service::CreateSession()
{
    shared_ptr<Session> session = m_sessionFactory();
	session->SetService(shared_from_this());

	if (m_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(shared_ptr<Session> _session)
{
	WRITE_LOCK;

	m_sessionCount++;
	m_sessions.insert(_session);
}

void Service::ReleaseSession(shared_ptr<Session> _session)
{
	WRITE_LOCK;

	ASSERT_CRASH(m_sessions.erase(_session) != 0);
	m_sessionCount--;
}

ServerService::ServerService(NetAddress _netAddress, shared_ptr<IocpCore> _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount)
	: Service(_netAddress, _iocpCore, _sessionFactory, _maxSessionCount)
{
	m_isServerService = true;
}

ServerService::~ServerService()
{
}

bool ServerService::Start()
{
	m_listener = make_shared<Listener>();
	if (m_listener == nullptr)
		return false;

    if (m_listener->StartAccept(static_pointer_cast<ServerService>(shared_from_this())) == false)
		return false;

	cout << "Server Start!" << endl;

	return true;
}

void ServerService::End()
{
}

ClientService::ClientService(NetAddress _netAddress, shared_ptr<IocpCore> _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount)
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
		shared_ptr<Session> session = CreateSession();
		if (session->Connect() == false)
			return false;
	}

	return true;
}

void ClientService::End()
{
}