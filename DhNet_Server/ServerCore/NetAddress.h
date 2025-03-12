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
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, unsigned __int16 port);

	SOCKADDR_IN&		GetSockAddr() { return _sockAddr; }
	wstring				GetIpAddress();
	unsigned __int16	GetPort() { return ::ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR		Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN			_sockAddr = {}; // 
};