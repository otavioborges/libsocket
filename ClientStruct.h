#ifndef CLIENTSTRUCT_H_
#define CLIENTSTRUCT_H_

#include <string>
#include <vector>
#include <utility>
#include <stdint.h>
#include <netinet/in.h>
#include <pthread.h>
#include "socrates.h"
#include "SocketServerCallback.h"

#define BUFFER_SIZE 1024

class socrates::ClientStruct {
	private:
		static pthread_mutex_t RECEIVED_MUTEX;
		static pthread_mutex_t SEND_MUTEX;
		static pthread_mutex_t DISCONNECTED_MUTEX;

		typedef struct {
			socrates::ClientStruct *instance;
			int clientSocket;
			bool connected;
			socrates::SocketServerCallback *callback;
			uint8_t buffer[BUFFER_SIZE];
			struct sockaddr_in addr;
			int error;
			std::vector<std::pair<uint16_t, uint8_t *>> bufferSendMsgs;
		}thread_args_t;

		pthread_t m_recvThread;
		pthread_t m_sendThread;
		thread_args_t m_threadArgs;

		static void *RecvRoutine(void *args);
		static void *SendRoutine(void *args);
	public:
		ClientStruct(int clientSocket, struct sockaddr_in addr, socrates::SocketServerCallback &callback);
		~ClientStruct(void);
		void Close(void);
		bool Connected(void);
		struct sockaddr_in *GetAddress(void);

		// Sending msg
		int SendMessage(const uint8_t *msg, uint16_t length, bool blocking);
		int SendMessage(std::string msg, bool blocking);
		int PendingMessages(void);
};

#endif /* CLIENTSTRUCT_H_ */