#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "..\\x64\\Debug\\ServerCore.lib")
#else
#pragma comment(lib, "..\\x64\\Release\\ServerCore.lib")
#endif

#include "../ServerCore/pch.h"

#include "ServerGlobal.h"

#include <chrono>
#include <thread>

#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>

#define MAX_ROOM_PLAYER 2