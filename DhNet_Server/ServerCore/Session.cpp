#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
#include "Sender.h"

/*--------------
	Session
---------------*/

Session::Session() : m_recvBuffer(BUFFER_SIZE)
{
	m_socket = SocketUtils::CreateSocket();
	Sender::Init(1024);
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
		ProcessSend(_numOfBytes);
		break;
	default:
		break;
	}
}

void Session::Send(shared_ptr<SendBuffer> _sendBuffer)
{
	WRITE_LOCK;

	m_sendQueue.push(_sendBuffer);

	/*if (m_sendRegistered == false)
	{
		m_sendRegistered = true;
		RegisterSend();
	}*/

	// ���� RegisterSend�� �ɸ��� ���� ���¶��, �ɾ��ش�.
	if (m_sendRegistered.exchange(true) == false)
	{
		RegisterSend_Old();
	}
}

void Session::Send(shared_ptr<Sender> _sender)
{
	WRITE_LOCK;

	m_senderQueue.push(_sender);

	// ���� RegisterSend�� �ɸ��� ���� ���¶��, �ɾ��ش�.
	if (m_sendRegistered.exchange(true) == false)
	{
		RegisterSend();
	}
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* _cause)
{
	if (m_connected.exchange(false) == false)
		return;

	// TEMP
	wcout << "Disconnect : " << _cause << endl;

	OnDisconnected(); // ������ �ڵ忡�� ������
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

	if (SocketUtils::BindAnyAddress(m_socket, 0/*���°�*/) == false)
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
	wsaBuf.buf = reinterpret_cast<char*>(m_recvBuffer.WritePos());
	wsaBuf.len = m_recvBuffer.FreeSize();

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

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	m_sendEvent.Init();
	m_sendEvent.m_owner = shared_from_this();

	// ���� �����͸� sendEvent�� ���
	{
		WRITE_LOCK;

		int32 writeSize = 0;
		while (m_senderQueue.empty() == false)
		{
			shared_ptr<Sender> sender = m_senderQueue.front();

			writeSize += sender->GetSendSize();
			// TODO : ���� üũ

			m_sendQueue.pop();
			m_sendEvent.m_senders.push_back(sender);
		}
	}

	// Scatter-Gather (����� �ִ� �����͵��� ��Ƽ� �� �濡 ������.)
	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(m_sendEvent.m_sendBuffers.size());
	for (shared_ptr<Sender> sender : m_sendEvent.m_senders)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sender->GetSendPointer());
		wsaBuf.len = static_cast<LONG>(sender->GetSendSize());
		wsaBufs.emplace_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(m_socket, wsaBufs.data(),
		static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &m_sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			m_sendEvent.m_owner = nullptr; // RELEASE_REF
			m_sendEvent.m_senders.clear(); // RELEASE_REF
			m_sendRegistered.store(false);
		}
	}
}

void Session::RegisterSend_Old()
{
	if (IsConnected() == false)
		return;

	m_sendEvent.Init();
	m_sendEvent.m_owner = shared_from_this();

	// ���� �����͸� sendEvent�� ���
	{
		WRITE_LOCK;

		int32 writeSize = 0;
		while (m_sendQueue.empty() == false)
		{
			shared_ptr<SendBuffer> sendBuffer = m_sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO : ���� üũ

			m_sendQueue.pop();
			m_sendEvent.m_sendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather (����� �ִ� �����͵��� ��Ƽ� �� �濡 ������.)
	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(m_sendEvent.m_sendBuffers.size());
	for (shared_ptr<SendBuffer> sendBuffer : m_sendEvent.m_sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.emplace_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(m_socket, wsaBufs.data(),
		static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &m_sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			m_sendEvent.m_owner = nullptr; // RELEASE_REF
			m_sendEvent.m_sendBuffers.clear(); // RELEASE_REF
			m_sendRegistered.store(false);
		}
	}
}

void Session::ProcessConnect()
{
	m_connectEvent.m_owner = nullptr; // RELEASE_REF

	m_connected.store(true);

	// ���� ���
	GetService()->AddSession(GetSessionRef());

	// ������ �ڵ忡�� ������
	OnConnected();

	// ���� ���
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	m_disconnectEvent.m_owner = nullptr; // RELEASE_REF
}

void Session::ProcessRecv(int32 _numOfBytes)
{
	m_recvEvent.m_owner = nullptr; // RELEASE_REF

	if (_numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (m_recvBuffer.OnWrite(_numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = m_recvBuffer.DataSize();

	// ������ �ڵ忡�� ������
	int32 processLen = OnRecv(m_recvBuffer.ReadPos(), dataSize);

	if (processLen < 0 || dataSize < processLen
		|| m_recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	// Ŀ�� ����
	m_recvBuffer.Clean();

	// ���� ���
	RegisterRecv();
}

void Session::ProcessSend(int32 _numOfBytes)
{
	m_sendEvent.m_owner = nullptr; // RELEASE_REF
	m_sendEvent.m_sendBuffers.clear(); // RELEASE_REF

	if (_numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// ������ �ڵ忡�� ������
	OnSend(_numOfBytes);

	WRITE_LOCK;
	if (m_sendQueue.empty())
	{
		m_sendRegistered.store(false);
	}
	else
	{
		RegisterSend();
	}
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