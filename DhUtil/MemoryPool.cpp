#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 _allocSize) : m_allocSize(_allocSize)
{
	::InitializeSListHead(&m_header);
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(
		::InterlockedPopEntrySList(&m_header)))
	{
		// null이 아니면
		::_aligned_free(memory);
	}
}

void MemoryPool::Push(MemoryHeader* _ptr)
{
	_ptr->allocSize = 0;

	::InterlockedPushEntrySList(&m_header,
		static_cast<PSLIST_ENTRY>(_ptr));

	m_useCount.fetch_sub(1);
	m_reservedCount.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(
		::InterlockedPopEntrySList(&m_header));

	// 없으면 새로 생성
	if (memory == nullptr)
	{
		memory = reinterpret_cast<MemoryHeader*>(
			::_aligned_malloc(m_allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0);
		m_reservedCount.fetch_sub(1);
	}

	m_useCount.fetch_add(1);

	return memory;
}
