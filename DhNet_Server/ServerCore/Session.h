#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

/*----------------------------
			Session
------------------------------*/

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

private:
	weak_ptr<Service>	m_service;
	SOCKET				m_socket = INVALID_SOCKET;
	NetAddress			m_netAddress = {};
	atomic<bool>		m_connected = false;

private:
	RecvEvent			m_recvEvent;
	SendEvent 			m_sendEvent;	

public:
	// TEMP
	char m_recvBuffer[1000];

public:
	Session();
	virtual ~Session();

public:
	shared_ptr<Service> GetService()								{ return m_service.lock(); }
	void				SetService(shared_ptr<Service> _service)	{ m_service = _service; }

	void				SetNetAddress(NetAddress _address)			{ m_netAddress = _address; }
	NetAddress			GetAddress()								{ return m_netAddress; }
	SOCKET				GetSocket()									{ return m_socket; }

	bool				IsConnected()								{ return m_connected; }

	// 인터페이스
public:
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) override;

private:
	void				Disconnect(const WCHAR* _cause);

	void				RegisterRecv();
	void				RegisterSend();

	void				ProcessConnect();
	void				ProcessRecv(int32 _numOfBytes);
	void				ProcessSend(int32 _numOfBytes);

	void				HandleError(int32 _errorCode);

	// 오버로딩용
protected:
	virtual void		OnConnected() {}
	virtual int32		OnRecv(BYTE* _buffer, int32 _len) { return _len; }
	virtual void		OnSend(int32 _len) {}
	virtual void		OnDisconnected() {}
};