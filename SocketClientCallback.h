#ifndef SOCKETCLIENTCALLBACK_H_
#define SOCKETCLIENTCALLBACK_H_

#include <stdint.h>
#include "net.h"
#include "ClientStruct.h"

class net::SocketClientCallback {
public:
	virtual void Connected(net::SocketClient *client) = 0;
	virtual void Disconnected(net::SocketClient *client) = 0;
	virtual void ReceivedMessage(net::SocketClient *client, uint8_t *msg, uint16_t size) = 0;
	virtual void SendCallback(net::SocketClient *client, int sent) {};
};

#endif /* SOCKETCLIENTCALLBACK_H_ */
