#pragma once
#include <Windows.h>

enum
{
	SLIST_ALIGNMENT = 16
};

/*----------------------------
		 MemoryHeader
------------------------------*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	// [MemoryHeader][Data]

	MemoryHeader(int32 _size) : allocSize(_size) {}

	static void* AttachHeader(MemoryHeader* _header, int32 _size)
	{
		new(_header)MemoryHeader(_size); // placement new
		return reinterpret_cast<void*>(++_header); // [Data] 반환
	}

	static MemoryHeader* DetachHeader(void* _ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(_ptr) - 1;
		return header;
	}

	int32 allocSize; // 메모리 영역 크기
};

/*----------------------------
		  MemoryPool
------------------------------*/

// - Windows 쪽은 메모리 풀링을 안해도 괜찮다. 
// (버전이 올라가며 메모리 파편화 관련된 문제들이 많이 개선됨)
// - Linux 쪽은 여전히 느려 메모리 풀을 사용

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
private:
	SLIST_HEADER	m_header; // 메모리 영역들 보관

	int32			m_allocSize = 0; // 메모리 영역 크기
	atomic<int32>	m_useCount = 0; // 사용 중인 메모리 영역 개수
	atomic<int32>	m_reservedCount = 0; // 보관 중인 메모리 영역 개수

public:
	MemoryPool(int32 _allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* _ptr);
	MemoryHeader*	Pop();
};