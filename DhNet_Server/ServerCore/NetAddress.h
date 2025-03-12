#pragma once

/*--------------
	NetAddress
---------------*/

// ��Ĺ �ּ� ������ ���� �Լ� ����
// IP�ּҿ� ��Ʈ��ȣ�� �����ϰ� IP �ּҸ� string������ �ּҷ� ��ȯ

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN _sockAddr);
	NetAddress(wstring _ip, unsigned __int16 _port);

	SOCKADDR_IN&		GetSockAddr() { return m_sockAddr; }
	wstring				GetIpAddress();
	unsigned __int16	GetPort() { return ::ntohs(m_sockAddr.sin_port); }

public:
	static IN_ADDR		Ip2Address(const WCHAR* _ip);

private:
	SOCKADDR_IN			m_sockAddr = {};
};