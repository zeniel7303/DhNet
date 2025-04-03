#pragma once
#include "IocpCore.h"
#include "Sender.h"

class Session;

enum class EventType : unsigned __int8
{
	Connect,
	Disconnect,
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
	EventType			m_type;
	IocpObjectRef		m_owner;

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
 DisconnectEvent
---------------*/

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};

/*--------------
   AcceptEvent
---------------*/

class AcceptEvent : public IocpEvent
{
public:
	SessionRef m_session = nullptr;

public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}
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
	std::vector<SenderRef>		m_senders;

public:
	SendEvent() : IocpEvent(EventType::Send) {}
};