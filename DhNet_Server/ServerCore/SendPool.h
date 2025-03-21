#pragma once

struct DataChunk
{
	char mBuffer[1024];
};

class Sender;

class SendPool
{
private:
	USE_LOCK;

	DataChunk*				m_chunk;
	vector<Sender>			m_senderList;
	vector<bool>			m_flag;
	unsigned short			m_chunkCount;
	unsigned short			m_useSize;

public:
	SendPool();
	~SendPool();

	int Init(unsigned short _count);
	shared_ptr<Sender> Alloc(unsigned short _sendSize);
	bool DeAlloc(int _index, unsigned short _count);

	int GetIndex(unsigned short _count);
	unsigned short GetUsableSize();
};