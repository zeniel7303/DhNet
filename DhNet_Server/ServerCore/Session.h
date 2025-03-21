#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"

/*--------------
	 Session
---------------*/

class Sender;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000,	// 64kb
	};

private:
	USE_LOCK;

	weak_ptr<Service>				m_service;
	SOCKET							m_socket = INVALID_SOCKET;
	NetAddress						m_netAddress = {};
	atomic<bool>					m_connected = false;

private:
	ConnectEvent					m_connectEvent;
	DisconnectEvent					m_disconnectEvent;
	RecvEvent						m_recvEvent;
	SendEvent 						m_sendEvent;	

private:
	RecvBuffer						m_recvBuffer;

	queue<shared_ptr<SendBuffer>>	m_sendQueue;
	queue<shared_ptr<Sender>>		m_senderQueue;
	atomic<bool>					m_sendRegistered = false;	

public:
	Session();
	virtual ~Session();

public:
	shared_ptr<Service>				GetService()								{ return m_service.lock(); }
	void							SetService(shared_ptr<Service> _service)	{ m_service = _service; }

	void							SetNetAddress(NetAddress _address)			{ m_netAddress = _address; }
	NetAddress						GetAddress()								{ return m_netAddress; }
	SOCKET							GetSocket()									{ return m_socket; }
	shared_ptr<Session>				GetSessionRef()								{ return static_pointer_cast<Session>(shared_from_this()); }

	bool							IsConnected()								{ return m_connected; }

	// 인터페이스
public:
	virtual HANDLE					GetHandle() override;
	virtual void					Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) override;

public:
	void							Send(shared_ptr<SendBuffer> _sendBuffer);
	void							Send(shared_ptr<Sender> _sender);
	bool							Connect();
	void							Disconnect(const WCHAR* _cause);

private:
	bool							RegisterConnect();
	bool							RegisterDisconnect();
	void							RegisterRecv();
	void							RegisterSend();
	void							RegisterSend_Old();

	void							ProcessConnect();
	void							ProcessDisconnect();
	void							ProcessRecv(int32 _numOfBytes);
	void							ProcessSend(int32 _numOfBytes);

	void							HandleError(int32 _errorCode);

	// 오버로딩용
protected:
	virtual void					OnConnected() {}
	virtual int32					OnRecv(BYTE* _buffer, int32 _len) { return _len; }
	virtual void					OnSend(int32 _len) {}
	virtual void					OnDisconnected() {}
};