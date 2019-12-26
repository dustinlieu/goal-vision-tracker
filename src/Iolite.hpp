#ifndef Iolite_hpp
#define Iolite_hpp

#include <enet/enet.h>

#include "PacketHandler.hpp"
#include "Packets.h"

class PacketHandler;

class Iolite {
	public:
		Iolite(const char* host, int port);
		~Iolite();

		void SendPacket(const void* data, size_t size);
		void Update();

	private:
		ENetHost* m_Client;
		ENetPeer* m_Peer;

		PacketHandler* m_PacketHandler;
};

#endif
