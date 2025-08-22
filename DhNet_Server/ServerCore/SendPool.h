#pragma once

struct DataChunk
{
	char mBuffer[1024];
};

class Sender;

class SendPool
{
private:
	USE_LOCK

	DataChunk*				m_chunk;
	std::vector<Sender>		m_senderList;
	std::vector<bool>		m_useFlag;
	unsigned short			m_chunkCount;
	unsigned short			m_useSize;

public:
	SendPool();
	~SendPool();

	int Init(unsigned short _count);
	SenderRef Alloc(unsigned short _sendSize);
	bool DeAlloc(int _index, unsigned short _count);

	int GetAllocIndex(unsigned short _count);
	unsigned short GetUsableSize();
};