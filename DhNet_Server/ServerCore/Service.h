#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include "Session.h"
#include <functional>

/*--------------
	 Service
---------------*/

using SessionFactory = std::function<SessionRef(void)>;

class Service : public std::enable_shared_from_this<Service>
{
protected:
	USE_LOCK;

	NetAddress					m_netAddress;
	IocpCoreRef		m_iocpCore;

	std::set<SessionRef>	m_sessions;
	int32						m_sessionCount = 0;
	int32						m_maxSessionCount = 0;
	SessionFactory				m_sessionFactory;
	bool						m_isServerService;

public:
	Service(NetAddress _netAddress, IocpCoreRef _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount = 1);
	virtual ~Service();

	virtual bool Start() abstract;
	virtual void End() abstract;

	SessionRef CreateSession();
	void AddSession(SessionRef _session);
	void ReleaseSession(SessionRef _session);
	void BroadCast(SenderRef _sender);

	NetAddress& GetNetAddress() { return m_netAddress; }
	IocpCoreRef GetIocpCore() { return m_iocpCore; }
	bool IsServerService() { return m_isServerService; }
};

class ServerService : public Service
{
private:
	ListenerRef m_listener;

public:
	ServerService(NetAddress _netAddress, IocpCoreRef _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount = 1);
	~ServerService();

	virtual bool Start() override;
	virtual void End() override;
};

class ClientService : public Service
{
private:

public:
	ClientService(NetAddress _netAddress, IocpCoreRef _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount = 1);
	~ClientService();

	virtual bool Start() override;
	virtual void End() override;
};