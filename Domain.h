#ifndef DOMAIN_H_
#define DOMAIN_H_

#include <stdint.h>

namespace socket {
	class SocketType;
}

class SocketType {
public:
	static const int SockStream = 1;
	static const int SockDgram = 2;
	static const int SockRaw = 3;
	static const int SockRdm = 4;
	static const int SockConnDgram = 5;
};

#endif /* DOMAIN_H_ */