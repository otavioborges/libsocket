#ifndef SOCKETCLIENT_H_
#define SOCKETCLIENT_H_

#include <string>
#include <vector>
#include <utility>
#include <stdint.h>
#include <netinet/in.h>
#include <pthread.h>
#include "socrates.h"
#include "SocketFamily.h"
#include "SocketType.h"
#include "SocketClientCallback.h"

#define BUFFER_SIZE 1024

class socrates::SocketClient {
	private:
		static pthread_mutex_t RECEIVED_MUTEX;
		static pthread_mutex_t SEND_MUTEX;
		static pthread_mutex_t DISCONNECTED_MUTEX;

		typedef struct {
			int socket;
			socrates::SocketClient *instance;
			bool connected;
			socrates::SocketClientCallback *callback;
			uint8_t buffer[BUFFER_SIZE];
			struct sockaddr_in serverAddr;
			std::vector<std::pair<uint16_t, uint8_t *>> bufferSendMsgs;
		}client_args_t;

		pthread_t m_recvThread;
		pthread_t m_sendThread;
		client_args_t m_threadArgs;
		bool m_isValid;

		static void *RecvRoutine(void *args);
		static void *SendRoutine(void *args);
	public:
		SocketClient(std::string server, uint16_t port, socrates::SocketFamily family, socrates::SocketType type);
		SocketClient(in_addr_t server, uint16_t port, socrates::SocketFamily family, socrates::SocketType type);
		~SocketClient(void);
		bool Connect(void);
		void Close(void);
		bool Connected(void);
		void SetupCallbacks(socrates::SocketClientCallback &callback);
		struct sockaddr_in *GetServerAddress(void);

		// Sending msg
		int SendMessage(const uint8_t *msg, uint16_t length, bool blocking);
		int SendMessage(std::string msg, bool blocking);
		int PendingMessages(void);
};

#endif /* SOCKETCLIENT_H_ */
