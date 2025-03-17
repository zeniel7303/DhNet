#pragma once
#include <stack>

extern thread_local unsigned __int32 LThreadId;

extern thread_local stack<int32> LLockStack;