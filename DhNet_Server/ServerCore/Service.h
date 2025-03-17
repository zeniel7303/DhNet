#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include "Session.h"
#include <functional>

/*--------------
	 Service
---------------*/

using SessionFactory = function<shared_ptr<Session>(void)>;

class Service : public enable_shared_from_this<Service>
{
protected:
	USE_LOCK;

	NetAddress					m_netAddress;
	shared_ptr<IocpCore>		m_iocpCore;

	set<shared_ptr<Session>>	m_sessions;
	int32						m_sessionCount = 0;
	int32						m_maxSessionCount = 0;
	SessionFactory				m_sessionFactory;
	bool						m_isServerService;

public:
	Service(NetAddress _netAddress, shared_ptr<IocpCore> _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount = 1);
	virtual ~Service();

	virtual bool Start() abstract;
	virtual void End() abstract;

	shared_ptr<Session> CreateSession();
	void AddSession(shared_ptr<Session> _session);
	void ReleaseSession(shared_ptr<Session> _session);

	NetAddress& GetNetAddress() { return m_netAddress; }
	shared_ptr<IocpCore> GetIocpCore() { return m_iocpCore; }
	bool IsServerService() { return m_isServerService; }
};

class ServerService : public Service
{
private:
	shared_ptr<Listener> m_listener;

public:
	ServerService(NetAddress _netAddress, shared_ptr<IocpCore> _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount = 1);
	~ServerService();

	virtual bool Start() override;
	virtual void End() override;
};

class ClientService : public Service
{
private:

public:
	ClientService(NetAddress _netAddress, shared_ptr<IocpCore> _iocpCore, SessionFactory _sessionFactory, int32 _maxSessionCount = 1);
	~ClientService();

	virtual bool Start() override;
	virtual void End() override;
};