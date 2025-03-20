#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(int32 _bufferSize)
{
	m_buffer.resize(_bufferSize);
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::CopyData(void* _data, int32 _len)
{
	ASSERT_CRASH(Capacity() >= _len);
	::memcpy(m_buffer.data(), _data, _len);
	m_writeSize = _len;
}