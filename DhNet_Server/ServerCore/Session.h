#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

/*----------------------------
			Session
------------------------------*/

class Session : public IocpObject
{
private:
	SOCKET			m_socket = INVALID_SOCKET;
	NetAddress		m_netAddress = {};
	atomic<bool>	m_connected = false;

public:
	// TEMP
	char m_recvBuffer[1000];

public:
	Session();
	virtual ~Session();

public:
	void		SetNetAddress(NetAddress _address) { m_netAddress = _address; }
	NetAddress	GetAddress() { return m_netAddress; }
	SOCKET		GetSocket() { return m_socket; }

public:
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) override;
};