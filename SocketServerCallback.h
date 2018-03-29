#ifndef SOCKETSERVERCALLBACK_H_
#define SOCKETSERVERCALLBACK_H_

#include <stdint.h>
#include "net.h"
#include "ClientStruct.h"

class net::SocketServerCallback {
	public:
		virtual void ConnectedClient(net::ClientStruct *client) = 0;
		virtual void DisconnectedClient(net::ClientStruct *client) = 0;
		virtual void ReceivedMessage(net::ClientStruct *client, uint8_t *msg, uint16_t size) = 0;
};

#endif /* SOCKETSERVERCALLBACK_H_ */
