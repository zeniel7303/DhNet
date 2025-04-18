#include "pch.h"
#include "SocketUtils.h"

LPFN_CONNECTEX		SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init()
{
	// Winsock �ʱ�ȭ
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);

	// ��Ÿ�ӿ� �ּ� ������ API
	// https://learn.microsoft.com/ko-kr/windows/win32/api/mswsock/nf-mswsock-acceptex
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket,WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket,WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket,WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	Close(dummySocket);
}

void SocketUtils::Clear()
{
	::WSACleanup();
}

bool SocketUtils::BindWindowsFunction(SOCKET _socket, GUID _guid, LPVOID* _fn)
{
	DWORD bytes = 0;
	// https://learn.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsaioctl
	//								DummySocket
	return SOCKET_ERROR != ::WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &_guid, sizeof(_guid),
		_fn, sizeof(*_fn), OUT & bytes, NULL, NULL);
}

SOCKET SocketUtils::CreateSocket()
{
	// ::socket()�ʹ� �ٸ��� �Ʒ��� �Լ��� ����ȭ�� �ɼ��� ���� ������ ����.
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUtils::SetLinger(SOCKET _socket, unsigned __int16 _onoff, unsigned __int16 _linger)
{
	// Linger��
	// https://velog.io/@jyongk/TCP-%EC%86%8C%EC%BC%93-%EC%98%B5%EC%85%98-SOLINGER
	LINGER option;
	option.l_onoff = _onoff;
	option.l_linger = _linger;
	return SetSockOpt(_socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtils::SetReuseAddress(SOCKET _socket, bool _flag)
{
	return SetSockOpt(_socket, SOL_SOCKET, SO_REUSEADDR, _flag);
}

bool SocketUtils::SetRecvBufferSize(SOCKET _socket, __int32 _size)
{
	return SetSockOpt(_socket, SOL_SOCKET, SO_RCVBUF, _size);
}

bool SocketUtils::SetSendBufferSize(SOCKET _socket, __int32 _size)
{
	return SetSockOpt(_socket, SOL_SOCKET, SO_SNDBUF, _size);
}

bool SocketUtils::SetTcpNoDelay(SOCKET _socket, bool _flag)
{
	return SetSockOpt(_socket, SOL_SOCKET, TCP_NODELAY, _flag);
}

// ListenSocket�� Ư���� ClientSocket�� �״�� ����
bool SocketUtils::SetUpdateAcceptSocket(SOCKET _socket, SOCKET _listenSocket)
{
	return SetSockOpt(_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, _listenSocket);
}

bool SocketUtils::Bind(SOCKET& _socket, NetAddress _netAddr)
{
	return SOCKET_ERROR != ::bind(_socket,
		reinterpret_cast<const SOCKADDR*>(&_netAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

bool SocketUtils::BindAnyAddress(SOCKET& _socket, unsigned __int16 _port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(_port);

	return SOCKET_ERROR != ::bind(_socket,
		reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
}

bool SocketUtils::Listen(SOCKET& _socket, __int32 _backlog)
{
	return SOCKET_ERROR != listen(_socket, _backlog);
}

void SocketUtils::Close(SOCKET& _socket)
{
	if (_socket != INVALID_SOCKET)
		::closesocket(_socket);

	_socket = INVALID_SOCKET;
}