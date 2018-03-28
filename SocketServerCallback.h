#ifndef SOCKETSERVERCALLBACK_H_
#define SOCKETSERVERCALLBACK_H_

#include <stdint.h>
#include <netinet/in.h>

namespace net {
	class SocketServerCallback;
}

class net::SocketServerCallback {
	public:
		virtual void ConnectedClient(struct sockaddr_in clientAddr) = 0;
		virtual void ReceivedMessage(struct sockaddr_in *clientAddr, uint8_t *msg, uint16_t size) = 0;
};

#endif /* SOCKETSERVERCALLBACK_H_ */
