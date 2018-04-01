#ifndef SOCRATES_H_
#define SOCRATES_H_

namespace socrates {
#ifdef SERVER
	class ClientStruct;
	class ClientStructSet;
	class SocketServerCallback;
	class SocketServer;
#endif
	class SocketClientCallback;
	class SocketClient;

	enum class SocketType;
	enum class SocketFamily;
}

#endif /* SOCRATES_H_ */
