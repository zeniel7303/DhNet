#pragma once
#include "IocpCore.h"

class Session;

enum class EventType : unsigned __int8
{
	Connect,
	Accept,
	Recv,
	Send
};

/*--------------
	IocpEvent
---------------*/

class IocpEvent : public OVERLAPPED
{
public:
	EventType				m_type;
	shared_ptr<IocpObject>	m_owner;

public:
	IocpEvent(EventType _type);

	void		Init();
};

/*--------------
   ConnectEvent
---------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

/*--------------
   AcceptEvent
---------------*/

class AcceptEvent : public IocpEvent
{
private:
	Session* m_session = nullptr;

public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

	void		SetSession(Session* _session) { m_session = _session; }
	Session*	GetSession() { return m_session; }
};

/*--------------
	RecvEvent
---------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

/*--------------
	SendEvent
---------------*/

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}
};