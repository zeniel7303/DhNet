#pragma once

// ��Ĺ �ּ� ������ ���� �Լ� ����
// IP�ּҿ� ��Ʈ��ȣ�� �����ϰ� IP �ּҸ� string������ �ּҷ� ��ȯ

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN _sockAddr);
	NetAddress(std::wstring _ip, uint16 _port);

	SOCKADDR_IN&		GetSockAddr() { return m_sockAddr; }
	std::wstring		GetIpAddress();
	uint16				GetPort() { return ::ntohs(m_sockAddr.sin_port); }

public:
	static IN_ADDR		Ip2Address(const WCHAR* _ip);

private:
	SOCKADDR_IN			m_sockAddr = {};
};