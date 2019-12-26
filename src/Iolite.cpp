#include "Iolite.hpp"

#include <iostream>
#include <string>

Iolite::Iolite(const char* host, int port) {
	enet_initialize();

	ENetAddress address;

	enet_address_set_host(&address, host);
	address.port = port;

	m_Client = enet_host_create(NULL, 1, 2, 0, 0);
	m_Peer = enet_host_connect(m_Client, &address, 2, 0);

	m_PacketHandler = new PacketHandler();
}

Iolite::~Iolite() {
	delete m_PacketHandler;
}

void Iolite::SendPacket(const void* data, size_t size) {
	ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_Peer, 0, packet);
}

void Iolite::Update() {
	ENetEvent event;

	while (enet_host_service(m_Client, &event, 0) > 0) {
		if (event.type == ENET_EVENT_TYPE_CONNECT) {
			std::cout << "CONNECTION!" << std::endl;
		} else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
			m_PacketHandler->Handle(this, event.packet);
			enet_packet_destroy(event.packet);
		}
	}
}