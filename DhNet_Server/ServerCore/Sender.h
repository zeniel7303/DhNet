#pragma once
#include "SendPool.h"
#include "PacketHeader.h"

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
	static SenderRef Alloc();
	int DeAlloc();

	void SetSendDataChunk(DataChunk* _chunk, int _chunkIndex, unsigned short _chunkCount);

	template<typename T>
	T* GetWritePointer();
	void* GetSendPointer();
	unsigned short GetSendSize();

	template<typename T>
	static std::pair<T*, SenderRef> GetSenderAndPacket();
};

template<typename T>
inline SenderRef Sender::Alloc()
{
	int size = sizeof(T);

	ASSERT_CRASH(size >= sizeof(PacketHeader));

	auto shared = m_sendPool.Alloc(size);

	if (shared == nullptr) return nullptr;

	(reinterpret_cast<PacketHeader*>(shared->m_tempChunk))->m_dataSize = size;

	return shared;
}

template<typename T>
inline T* Sender::GetWritePointer()
{
	auto ptr = reinterpret_cast<char*>(m_tempChunk);

	return reinterpret_cast<T*>(ptr);
}

template<typename T>
inline static std::pair<T*, SenderRef> Sender::GetSenderAndPacket()
{
	auto sender = Alloc<T>();
	auto packet = sender->GetWritePointer<T>();
	return make_pair(packet, sender);
}