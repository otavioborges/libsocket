#ifndef SOCKETTYPE_H_
#define SOCKETTYPE_H_

#include <cstdlib>
#include "net.h"

enum class net::SocketType {
	SockStream		= 1,
	SockDgram		= 2,
	SockRaw			= 3,
	SockRdm			= 4,
	SockConnDgram	= 5
};

#endif /* SOCKETTYPE_H_ */
