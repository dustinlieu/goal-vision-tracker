#ifndef PacketHandler_hpp
#define PacketHandler_hpp

#include <enet/enet.h>
#include <stdio.h>

#include "Iolite.hpp"
#include "Packets.h"

class Iolite;

class PacketHandler {
	public:
		PacketHandler();

		IncomingPacket::OperationCodes ParseHeader(ENetPacket* packet);
		void Handle(Iolite* iolite, ENetPacket* packet);
};

#endif