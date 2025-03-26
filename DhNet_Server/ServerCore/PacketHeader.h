#pragma once
#include "pch.h"

#pragma pack(push, 1)

class PacketHeader
{
public:
	unsigned short m_packetNum;
	unsigned short m_dataSize;

public:
	void Init(UINT16 _num, unsigned short _size)
	{
		m_packetNum = _num;
		m_dataSize = _size;
	}
};

#pragma pack(pop)