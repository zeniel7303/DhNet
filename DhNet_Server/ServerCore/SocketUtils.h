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

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static SOCKET CreateSocket();

	// ���� ����ϴ� ��ɵ� �̸� ����
	// https://velog.io/@jyongk/TCP-%EC%86%8C%EC%BC%93-%EC%98%B5%EC%85%98-SOLINGER
	static bool SetLinger(SOCKET socket, unsigned __int16 onoff, unsigned __int16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, __int32 size);
	static bool SetSendBufferSize(SOCKET socket, __int32 size);
	// ���̱� �˰���
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	static bool Bind(SOCKET socket, NetAddress netAddr);
	static bool BindAnyAddress(SOCKET socket, unsigned __int16 port);
	static bool Listen(SOCKET socket, __int32 backlog = SOMAXCONN);
	static void Close(SOCKET& socket);
};

// SetSocketOption ����
template<typename T>
static inline bool SetSockOpt(SOCKET socket, __int32 level, __int32 optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}