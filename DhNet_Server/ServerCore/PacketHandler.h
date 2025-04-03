#pragma once
#include <functional>

#include "Handler.h"
#include "PacketHeader.h"

class Session;

using handleType = std::function<int(PacketHeader*, SessionRef&)>;

class PacketHandler : public Handler<handleType>
{
private:
	static PacketHandler m_singleton;

	PacketHandler() {}
public:
	~PacketHandler() {}

	static PacketHandler& Instance();
};