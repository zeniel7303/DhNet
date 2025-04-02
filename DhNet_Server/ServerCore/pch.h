#pragma once

#ifdef _DEBUG
#pragma comment(lib, "..\\..\\DhUtil\\x64\\Debug\\DhUtil.lib")
#else
#pragma comment(lib, "..\\..\\DhUtil\\x64\\Release\\DhUtil.lib")
#endif

#include "../../DhUtil/pch.h"

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <Windows.h>
#include <iostream>
using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <memory>
#include <vector>
#include <set>
#include <queue>
#include <map>