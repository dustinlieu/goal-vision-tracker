#include "PacketHandler.hpp"

#include <iostream>

PacketHandler::PacketHandler() {
}

IncomingPacket::OperationCodes PacketHandler::ParseHeader(ENetPacket* packet) {
	IncomingPacket::Generic* data = (IncomingPacket::Generic*) packet->data;
	IncomingPacket::OperationCodes opCode = static_cast<IncomingPacket::OperationCodes>(data->operationCode);

	return opCode;
}

void PacketHandler::Handle(Iolite* iolite, ENetPacket* packet) {
	IncomingPacket::OperationCodes opCode = ParseHeader(packet);

	if (opCode == IncomingPacket::OperationCodes::HEARTBEAT) {
		IncomingPacket::Heartbeat* data = (IncomingPacket::Heartbeat*) packet->data;
		OutgoingPacket::Heartbeat reply = OutgoingPacket::Heartbeat(data->nonce);

		iolite->SendPacket(&reply, sizeof(reply));
	} else if (opCode == IncomingPacket::OperationCodes::REQUEST_DATA) {
		IncomingPacket::RequestData* data = (IncomingPacket::RequestData*) packet->data;
		OutgoingPacket::CameraData reply = OutgoingPacket::CameraData(data->time);

		std::cout << reply.time << std::endl;

		iolite->SendPacket(&reply, sizeof(reply));
	}
}