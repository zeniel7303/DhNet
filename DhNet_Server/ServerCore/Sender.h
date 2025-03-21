#pragma once
#include "SendPool.h"

//패킷 전송시 사용
//SendPool에서 패킷 전송 할당받아 사용 후 반납.

class Sender
{
private:
	DataChunk*			m_tempChunk;
	int					m_index;
	unsigned short		m_count;

	static SendPool		m_sendPool;

public:
	Sender();
	~Sender();

	static int Init(unsigned short _sendCount);

	template<typename T>
	static shared_ptr<Sender> Alloc();
	int DeAlloc();

	void SetSendDataChunk(DataChunk* _chunk, int _chunkIndex, unsigned short _chunkCount);

	template<typename T>
	T* GetWritePointer();
	void* GetSendPointer();
	unsigned short GetSendSize();
};

template<typename T>
inline shared_ptr<Sender> Sender::Alloc()
{
	return shared_ptr<Sender>();
}

template<typename T>
inline T* Sender::GetWritePointer()
{
	return nullptr;
}
