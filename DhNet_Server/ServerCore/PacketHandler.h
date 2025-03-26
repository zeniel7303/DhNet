#pragma once
#include <functional>

#include "Handler.h"
#include "PacketHeader.h"

class Session;

using handleType = function<int(PacketHeader*, std::shared_ptr<Session>&)>;

class PacketHandler : public Handler<handleType>
{
private:
	static PacketHandler m_singleton;

	PacketHandler() {}
public:
	~PacketHandler() {}

	static PacketHandler& Instance();
};