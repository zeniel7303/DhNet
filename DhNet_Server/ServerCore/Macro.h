#pragma once

#define size16(val)	static_cast<__int16>(sizeof(val)):
#define size32(val)	static_cast<__int32>(sizeof(val)):
#define len16(arr)	static_cast<__int16>(sizeof(arr)/sizeof(arr[0]))
#define len32(arr)	static_cast<__int32>(sizeof(arr)/sizeof(arr[0]))

/*----------------------------
			Crash
------------------------------*/

#define CRASH(cause)					\
{										\
	unsigned __int32* crash = nullptr;	\
	__analysis_assume(crash != nullptr);\
	*crash = 0xDEADBEEF;				\
}

#define ASSERT_CRASH(expr)				\
{										\
	if(!(expr))							\
	{									\
		CRASH("ASSERT_CRASH");			\
		__analysis_assume(expr);		\
	}									\
}