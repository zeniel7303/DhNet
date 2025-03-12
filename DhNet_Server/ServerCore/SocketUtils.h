#pragma once
#include "NetAddress.h"

/*----------------
	SocketUtils
-----------------*/

// ���� ���� ��ƿ��Ƽ �޼ҵ带 ���� �� ���� ���� �ɼ� ���� Ŭ����

class SocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;

public:
	// �������� ȣ���� Init �� Clear
	static void Init();
	static void Clear();

	static bool BindWindowsFunction(SOCKET _socket, GUID _guid, LPVOID* _fn);
	static SOCKET CreateSocket();

	// ���� ����ϴ� ��ɵ� �̸� ����
	// https://velog.io/@jyongk/TCP-%EC%86%8C%EC%BC%93-%EC%98%B5%EC%85%98-SOLINGER
	static bool SetLinger(SOCKET _socket, unsigned __int16 _onoff, unsigned __int16 _linger);
	static bool SetReuseAddress(SOCKET _socket, bool _flag);
	static bool SetRecvBufferSize(SOCKET _socket, __int32 _size);
	static bool SetSendBufferSize(SOCKET _socket, __int32 _size);
	// ���̱� �˰���
	static bool SetTcpNoDelay(SOCKET _socket, bool _flag);
	static bool SetUpdateAcceptSocket(SOCKET _socket, SOCKET _listenSocket);

	static bool Bind(SOCKET& _socket, NetAddress _netAddr);
	static bool BindAnyAddress(SOCKET& _socket, unsigned __int16 _port);
	static bool Listen(SOCKET& _socket, __int32 _backlog = SOMAXCONN);
	static void Close(SOCKET& _socket);
};

// SetSocketOption ����
template<typename T>
static inline bool SetSockOpt(SOCKET _socket, __int32 _level, __int32 _optName, T _optVal)
{
	return SOCKET_ERROR != ::setsockopt(_socket, _level, _optName,
		reinterpret_cast<char*>(&_optVal), sizeof(T));
}