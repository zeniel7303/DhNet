#pragma once

#ifdef _DEBUG
#pragma comment(lib, "..\\..\\DhUtil\\x64\\Debug\\DhUtil.lib")
#else
#pragma comment(lib, "..\\..\\DhUtil\\x64\\Release\\DhUtil.lib")
#endif

#include "../../DhUtil/pch.h"
#include "../../DhUtil/Lock.h"

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#include <Windows.h>
#include <iostream>
using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <memory>
#include <set>