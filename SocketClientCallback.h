#ifndef SOCKETCLIENTCALLBACK_H_
#define SOCKETCLIENTCALLBACK_H_

#include <stdint.h>
#include "socrates.h"
#include "SocketClient.h"

class socrates::SocketClientCallback {
public:
	virtual void Connected(socrates::SocketClient *client) = 0;
	virtual void Disconnected(socrates::SocketClient *client) = 0;
	virtual void ReceivedMessage(socrates::SocketClient *client, uint8_t *msg, uint16_t size) = 0;
	virtual void SendCallback(socrates::SocketClient *client, int sent) {};
};

#endif /* SOCKETCLIENTCALLBACK_H_ */
