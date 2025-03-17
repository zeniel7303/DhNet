#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

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

void Session::Dispatch(IocpEvent* _iocpEvent, int32 _numOfBytes)
{
	switch (_iocpEvent->m_type)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Recv:
		ProcessRecv(_numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(_numOfBytes);
		break;
	default:
		break;
	}
}

void Session::Disconnect(const WCHAR* _cause)
{
	if (m_connected.exchange(false) == false)
		return;

	cout << "Disconnect : " << _cause << endl;

	OnDisconnected();
	SocketUtils::Close(m_socket);
	GetService()->ReleaseSession(static_pointer_cast<Session>(shared_from_this()));
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	m_recvEvent.m_owner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(m_recvBuffer);
	wsaBuf.len = len32(m_recvBuffer);

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(m_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags,
		&m_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			m_recvEvent.m_owner = nullptr;	// Release Referance
		}
	}
}

void Session::RegisterSend()
{
	// TODO
}

void Session::ProcessConnect()
{
	cout << "Client Connected!" << endl;

	m_connected.store(true);

	// 세션 등록
	GetService()->AddSession(static_pointer_cast<Session>(shared_from_this()));

	// 컨탠츠 코드에서 오버로딩
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessRecv(int32 _numOfBytes)
{
	m_recvEvent.m_owner = nullptr;

	if (_numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	// TODO
	cout << "Recv Data Len = " << _numOfBytes << endl;

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessSend(int32 _numOfBytes)
{
}

void Session::HandleError(int32 _errorCode)
{
	switch (_errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		cout << "Handle Error : " << _errorCode << endl;
		break;
	}
}