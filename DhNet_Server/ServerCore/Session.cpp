#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
#include "Sender.h"

Session::Session() : m_recvBuffer(BUFFER_SIZE)
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
		ProcessSend(_numOfBytes);
		break;
	default:
		break;
	}
}

void Session::Send(SenderRef _sender)
{
	WRITE_LOCK;

	m_senderQueue.push(_sender);

	// 현재 RegisterSend가 걸리지 않은 생태라면, 걸어준다.
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
	std::cout << "Disconnect : " << _cause << std::endl;

	OnDisconnected(); // 컨텐츠 코드에서 재정의
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

	// 보낼 데이터를 sendEvent에 등록
	{
		WRITE_LOCK;

		while (m_senderQueue.empty() == false)
		{
			SenderRef sender = m_senderQueue.front();

			m_senderQueue.pop();
			m_sendEvent.m_senders.push_back(sender);
		}
	}

	// Scatter-Gather (흩어져 있는 데이터들을 모아서 한 방에 보낸다.)
	std::vector<WSABUF> wsaBufs;
	wsaBufs.reserve(m_sendEvent.m_senders.size());
	for (SenderRef sender : m_sendEvent.m_senders)
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

void Session::ProcessConnect()
{
	m_connectEvent.m_owner = nullptr; // RELEASE_REF

	m_connected.store(true);

	// 세션 등록
	GetService()->AddSession(GetSessionRef());

	// 컨탠츠 코드에서 재정의
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

	while (1)
	{
		auto packet = reinterpret_cast<PacketHeader*>(m_recvBuffer.ReadPos());
		if (m_recvBuffer.DataSize() >= packet->m_dataSize)
		{
			// 컨텐츠 코드에서 재정의
			auto result = OnRecv(packet);
			if (result == false)
			{
				Disconnect(L"OnRead Error");
			}

			if (m_recvBuffer.OnRead(packet->m_dataSize) == false)
			{
				Disconnect(L"OnRead Overflow");
				return;
			}
		}

		break;
	}

	// 커서 정리
	m_recvBuffer.Clean();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessSend(int32 _numOfBytes)
{
	m_sendEvent.m_owner = nullptr; // RELEASE_REF
	m_sendEvent.m_senders.clear(); // RELEASE_REF

	if (_numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 코드에서 재정의
	OnSend(_numOfBytes);

	WRITE_LOCK;

	if (m_senderQueue.empty())
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
		std::cout << "Handle Error : " << _errorCode << std::endl;
		break;
	}
}