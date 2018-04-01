#ifndef SOCKETSERVERCALLBACK_H_
#define SOCKETSERVERCALLBACK_H_

#include <stdint.h>
#include "socrates.h"
#include "ClientStruct.h"

class socrates::SocketServerCallback {
	public:
		virtual void ConnectedClient(socrates::ClientStruct *client) = 0;
		virtual void DisconnectedClient(socrates::ClientStruct *client) = 0;
		virtual void ReceivedMessage(socrates::ClientStruct *client, uint8_t *msg, uint16_t size) = 0;
		virtual void SendCallback(socrates::ClientStruct *client, int sent) {};
};

#endif /* SOCKETSERVERCALLBACK_H_ */
