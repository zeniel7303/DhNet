#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "..\\x64\\Debug\\ServerCore.lib")
#pragma comment(lib, "..\\x64\\Debug\\DhUtil.lib")
#else
#pragma comment(lib, "..\\x64\\Release\\ServerCore.lib")
#pragma comment(lib, "..\\x64\\Release\\DhUtil.lib")
#endif

#include "../ServerCore/pch.h"