#ifndef CLIENTSTRUCT_H_
#define CLIENTSTRUCT_H_

#include <string>
#include <stdint.h>
#include <netinet/in.h>
#include <pthread.h>
#include "net.h"
#include "SocketServerCallback.h"

#define BUFFER_SIZE 1024

class net::ClientStruct {
	private:
		static pthread_mutex_t RECEIVED_MUTEX;
		static pthread_mutex_t DISCONNECTED_MUTEX;

		typedef struct {
			net::ClientStruct *instance;
			int clientSocket;
			bool connected;
			net::SocketServerCallback *callback;
			uint8_t buffer[BUFFER_SIZE];
			struct sockaddr_in addr;
			int error;
		}thread_args_t;

		pthread_t m_recvThread;
		thread_args_t m_threadArgs;

		static void *RecvRoutine(void *args);
	public:
		ClientStruct(int clientSocket, struct sockaddr_in addr, net::SocketServerCallback &callback);
		~ClientStruct(void);
		void Close(void);
		bool Connected(void);
		struct sockaddr_in *GetAddress(void);

		// Sending msg
		int SendMessage(const uint8_t *msg, uint16_t length);
		int SendMessage(std::string msg);
};

#endif /* CLIENTSTRUCT_H_ */