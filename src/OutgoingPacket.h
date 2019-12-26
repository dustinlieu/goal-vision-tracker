#ifndef OutgoingPacket_h
#define OutgoingPacket_h

namespace OutgoingPacket {
	enum OperationCodes {
		NOTHING = 0x00,
		HEARTBEAT = 0x01,
		CAMERA_DATA = 0x02
	};

	struct Generic {
		Generic() {
			protocol = PROTOCOL_VERSION;
			operationCode = OperationCodes::NOTHING;
		}

		int protocol;
		int operationCode;
	};

	struct Heartbeat : Generic {
		Heartbeat(int reply) {
			operationCode = OperationCodes::HEARTBEAT;
			nonce = reply;
		}

		int nonce;
	};

	struct CameraData : Generic {
		CameraData(long long reply) {
			operationCode = OperationCodes::CAMERA_DATA;
			time = reply;
			x = 0;
			y = 0;
			leftDist = 0;
			rightDist = 0;
		}

		long long time;
		double x;
		double y;
		double leftDist;
		double rightDist;
	};
}

#endif
