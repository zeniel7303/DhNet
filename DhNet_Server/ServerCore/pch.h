#pragma once

#ifdef _DEBUG
#pragma comment(lib, "..\\..\\DhUtil\\x64\\Debug\\DhUtil.lib")
#else
#pragma comment(lib, "..\\..\\DhUtil\\x64\\Release\\DhUtil.lib")
#endif

#include "../../DhUtil/pch.h"

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#include <iostream>
#include <memory>
#include <cstdlib>

#include <Windows.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <set>
#include <queue>
#include <map>

#include <string>

#include "Types.h"