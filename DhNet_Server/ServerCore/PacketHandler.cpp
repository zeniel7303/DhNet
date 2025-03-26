#include "PacketHandler.h"

PacketHandler PacketHandler::m_singleton;

PacketHandler& PacketHandler::Instance()
{
	return m_singleton;
}