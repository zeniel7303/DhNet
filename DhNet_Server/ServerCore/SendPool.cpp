#include "pch.h"
#include "SendPool.h"
#include "Sender.h"
#include <cassert>

SendPool::SendPool() : m_chunk(nullptr), m_chunkCount(0),
m_flag(), m_senderList(), m_useSize(0)
{

}

SendPool::~SendPool()
{
	delete[] m_chunk;
}

int SendPool::Init(unsigned short _count)
{
	ASSERT_CRASH(m_chunk == nullptr);
	ASSERT_CRASH(m_useSize == 0);

	try
	{
		m_chunk = new DataChunk[_count];

		m_chunkCount = _count;

		m_flag.reserve(_count);
		m_senderList.reserve(_count);
	}
	catch (...)
	{
		ASSERT_CRASH(0);
	}

	for (int i = 0; i < _count; ++i)
	{
		m_flag.emplace_back(false);
		m_senderList.emplace_back(Sender());
	}

	return 0;
}

std::shared_ptr<Sender> SendPool::Alloc(unsigned short _sendSize)
{
	int index = 0;
	unsigned short count = (_sendSize / sizeof(DataChunk)) + 1;

	Sender* sender = nullptr;
	DataChunk* sendChunk = nullptr;

	{
		WRITE_LOCK;

		index = GetIndex(count);

		if (index == -1) return nullptr;

		sender = &(m_senderList[index]);
		sendChunk = &(m_chunk[index]);

		for (int i = 0; i < (index + count); ++i)
		{
			m_flag[i] = true;
		}

		m_useSize += (count * sizeof(DataChunk));

		sender->SetSendDataChunk(sendChunk, index, count);
	}

	//c++ shared_ptr delete lambda
	//https://stackoverflow.com/questions/13633737/using-a-custom-deleter-for-stdshared-ptr-on-a-direct3d11-object
	return std::shared_ptr<Sender>(sender, [this](Sender* _sender)
		{
			// ½ÇÆÐ½Ã ÆÄ±«
			_sender->DeAlloc();
		});
}

bool SendPool::DeAlloc(int _index, unsigned short _count)
{
	ASSERT_CRASH(_index <= m_chunkCount);
	ASSERT_CRASH(_count < m_chunkCount);

	{
		WRITE_LOCK;

		for (int i = _index; i < (_index + _count); ++i)
		{
			if (m_flag[i] == false)
				return false;

			m_flag[i] = false;
			m_useSize -= sizeof(DataChunk);
		}
	}

	return true;
}

int SendPool::GetIndex(unsigned short _count)
{
	int filledCount = 0;
	int allocIndex = -1;
	int flagIndex = 0;

	for (const auto& flag : m_flag)
	{
		if (!flag)
		{
			allocIndex = flagIndex;

			++filledCount;

			if (filledCount == _count) break;
		}

		++flagIndex;
	}

	if (filledCount != _count) allocIndex = -1;

	return allocIndex;
}

unsigned short SendPool::GetUsableSize()
{
	READ_LOCK;

	return m_useSize;
}