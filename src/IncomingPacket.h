#ifndef IncomingPacket_h
#define IncomingPacket_h

namespace IncomingPacket {
	enum OperationCodes {
		NOTHING = 0x00,
		HEARTBEAT = 0x01,
		REQUEST_DATA = 0x02
	};

	struct Generic {
		int protocol;
		int operationCode;
	};

	struct Heartbeat {
		int nonce;
	};

	struct RequestData : Generic {
		long long time;
	};
}

#endif
