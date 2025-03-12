#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"

/*--------------
	Session
---------------*/

Session::Session()
{
	m_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(m_socket);
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(m_socket);
}

void Session::Dispatch(IocpEvent* _iocpEvent, __int32 _numOfBytes)
{
	// TODO
}