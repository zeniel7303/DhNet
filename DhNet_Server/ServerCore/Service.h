#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"

/*--------------
	 Service
---------------*/

class Service : public enable_shared_from_this<Service>
{
protected:
	NetAddress m_netAddress;
	shared_ptr<IocpCore> m_iocpCore;

public:
	Service(NetAddress _netAddress, shared_ptr<IocpCore> _iocpCore);
	virtual ~Service();

	virtual bool Start() abstract;
	virtual void End() abstract;

	NetAddress& GetNetAddress() { return m_netAddress; }
	shared_ptr<IocpCore> GetIocpCore() { return m_iocpCore; }
};

class ServerService : public Service
{
private:
	shared_ptr<Listener> m_listener;

public:
	ServerService(NetAddress _netAddress, shared_ptr<IocpCore> _iocpCore);
	~ServerService();

	virtual bool Start() override;
	virtual void End() override;
};

class ClientService : public Service
{
private:

public:
	ClientService(NetAddress _netAddress, shared_ptr<IocpCore> _iocpCore);
	~ClientService();

	virtual bool Start() override;
	virtual void End() override;
};