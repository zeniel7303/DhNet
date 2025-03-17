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
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(_numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(static_cast<SendEvent*>(_iocpEvent), _numOfBytes);
		break;
	default:
		break;
	}
}

void Session::Send(BYTE* _buffer, int32 _len)
{
	// 생각할 문제
	// 1) 버퍼 관리?
	// 2) sendEvent 관리? 단일? 여러개? WSASend 중첩?

	// TEMP
	SendEvent* sendEvent = new SendEvent;
	sendEvent->m_owner = shared_from_this(); // ADD_REF
	sendEvent->m_buffer.resize(_len);
	::memcpy(sendEvent->m_buffer.data(), _buffer, _len);

	WRITE_LOCK;
	RegisterSend(sendEvent);
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* _cause)
{
	if (m_connected.exchange(false) == false)
		return;

	cout << "Disconnect : " << _cause << endl;

	OnDisconnected();
	SocketUtils::Close(m_socket);
	GetService()->ReleaseSession(GetSessionRef());

	RegisterDisconnect();
}

bool Session::RegisterConnect()
{
	if (IsConnected()) return false;

	if (GetService()->IsServerService())
		return false;

	if (SocketUtils::SetReuseAddress(m_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(m_socket, 0/*남는거*/) == false)
		return false;

	m_connectEvent.Init();
	m_connectEvent.m_owner = shared_from_this(); // ADD_REF

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (SocketUtils::ConnectEx(m_socket, reinterpret_cast<SOCKADDR*>(&sockAddr),
		sizeof(sockAddr), nullptr, 0, &numOfBytes, &m_connectEvent) == false)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			m_connectEvent.m_owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect()
{
	m_disconnectEvent.Init();
	m_disconnectEvent.m_owner = shared_from_this(); // ADD_REF

	if (SocketUtils::DisconnectEx(m_socket,
		&m_disconnectEvent, TF_REUSE_SOCKET, 0) == false)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			m_disconnectEvent.m_owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
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
			m_recvEvent.m_owner = nullptr;	// RELEASE_REF
		}
	}
}

void Session::RegisterSend(SendEvent* _sendEvent)
{
	if (IsConnected() == false)
		return;

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)_sendEvent->m_buffer.data();
	wsaBuf.len = (ULONG)_sendEvent->m_buffer.size();

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(m_socket, &wsaBuf, 1, OUT & numOfBytes, 0, _sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent->m_owner = nullptr; // RELEASE_REF
			delete _sendEvent;
		}
	}
}

void Session::ProcessConnect()
{
	cout << "Client Connected!" << endl;

	m_connectEvent.m_owner = nullptr; // RELEASE_REF

	m_connected.store(true);

	// 세션 등록
	GetService()->AddSession(GetSessionRef());

	// 컨탠츠 코드에서 오버로딩
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	m_disconnectEvent.m_owner = nullptr; // RELEASE_REF
}

void Session::ProcessRecv(int32 _numOfBytes)
{
	m_recvEvent.m_owner = nullptr;

	if (_numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	OnRecv(m_recvBuffer, _numOfBytes);

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessSend(SendEvent* _sendEvent, int32 _numOfBytes)
{
	_sendEvent->m_owner = nullptr; // RELEASE_REF
	delete _sendEvent;

	if (_numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	OnSend(_numOfBytes);
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
		cout << "Handle Error : " << _errorCode << endl;
		break;
	}
}