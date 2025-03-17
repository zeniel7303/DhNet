#pragma once

//#include <iostream>
//using namespace std;
//
//#include <winsock2.h>
//#include <mswsock.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "ws2_32.lib")
//
//#include <thread>

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "..\\..\\DhNet_Server\\x64\\Debug\\ServerCore.lib")
#else
#pragma comment(lib, "..\\..\\DhNet_Server\\x64\\Release\\ServerCore.lib")
#endif

#include "../../DhNet_Server/ServerCore/pch.h"	