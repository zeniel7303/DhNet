#include "pch.h"
#include "Sender.h"

SendPool Sender::m_sendPool;

Sender::Sender() : m_tempChunk(nullptr), m_index(-1), m_count(0)
{

}

Sender::~Sender()
{

}

int Sender::Init(unsigned short _sendCount)
{
	return m_sendPool.Init(_sendCount);
}

int Sender::DeAlloc()
{
	if (m_tempChunk == nullptr) return 0;

	ASSERT_CRASH(m_index >= 0);
	ASSERT_CRASH(m_count > 0);

	const int index = m_index;
	const short count = m_count;

	m_tempChunk = nullptr;
	m_index = -1;
	m_count = 0;

	bool result = m_sendPool.DeAlloc(index, count);

	ASSERT_CRASH(result && "SendPool DeAlloc fail");

	return 0;
}

void Sender::SetSendDataChunk(DataChunk* _chunk, int _chunkIndex, unsigned short _chunkCount)
{
	ASSERT_CRASH(_chunk != nullptr);
	ASSERT_CRASH(_chunkIndex >= 0);
	ASSERT_CRASH(_chunkCount > 0);
	ASSERT_CRASH(m_tempChunk == nullptr);

	m_tempChunk = _chunk;
	m_index = _chunkIndex;
	m_count = _chunkCount;
}

void* Sender::GetSendPointer()
{
	return (reinterpret_cast<void*>(m_tempChunk));
}

unsigned short Sender::GetSendSize()
{
	if (m_tempChunk == nullptr)
		return 0;

	unsigned short dataSize =
		(reinterpret_cast<PacketHeader*>(m_tempChunk))->m_dataSize;

	if (dataSize == 0)
		return 0;

	return dataSize;

	return 0;
}