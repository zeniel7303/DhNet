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

class IocpEvent : public OVERLAPPED
{
public:
	EventType			m_type;
	IocpObjectRef		m_owner;

public:
	IocpEvent(EventType _type);

	void		Init();
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};

class AcceptEvent : public IocpEvent
{
public:
	SessionRef m_session = nullptr;

public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

class SendEvent : public IocpEvent
{
public:
	std::vector<SenderRef>		m_senders;

public:
	SendEvent() : IocpEvent(EventType::Send) {}
};