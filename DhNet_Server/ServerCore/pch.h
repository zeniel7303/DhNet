#pragma once

#include "CoreGlobal.h"

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <Windows.h>
#include <iostream>
using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "Macro.h"

#define size16(val)	static_cast<__int16>(sizeof(val)):
#define size32(val)	static_cast<__int32>(sizeof(val)):
#define len16(arr)	static_cast<__int16>(sizeof(arr)/sizeof(arr[0]))
#define len32(arr)	static_cast<__int32>(sizeof(arr)/sizeof(arr[0]))