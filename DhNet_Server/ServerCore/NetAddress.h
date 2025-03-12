#pragma once

/*--------------
	NetAddress
---------------*/

// 소캣 주소 정보와 관련 함수 제공
// IP주소와 포트번호를 관리하고 IP 주소를 string형태의 주소로 변환

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