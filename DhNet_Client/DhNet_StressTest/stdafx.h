#pragma once

#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#pragma comment(lib, "..\\..\\DhNet_Server\\x64\\Debug\\ServerCore.lib")
#else
#pragma comment(lib, "..\\..\\DhNet_Server\\x64\\Release\\ServerCore.lib")
#endif

#include "../../DhNet_Server/ServerCore/pch.h"