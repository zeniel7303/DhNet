#pragma once

/*--------------
	SendBuffer
---------------*/

class SendBuffer : enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(int32 _bufferSize);
	~SendBuffer();

	BYTE* Buffer() { return m_buffer.data(); }
	int32 WriteSize() { return m_writeSize; }
	int32 Capacity() { return static_cast<int32>(m_buffer.size()); }

	void CopyData(void* _data, int32 _len);

private:
	vector<BYTE>	m_buffer;
	int32			m_writeSize = 0;
};