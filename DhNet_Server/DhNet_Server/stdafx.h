#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "..\\x64\\Debug\\ServerCore.lib")
#else
#pragma comment(lib, "..\\x64\\Release\\ServerCore.lib")
#endif

#include "../ServerCore/pch.h"

#include "memory.h"