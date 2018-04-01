#ifndef SOCKETSERVER_H_
#define SOCKETSERVER_H_

#include <stdint.h>
#include <set>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "socrates.h"
#include "SocketFamily.h"
#include "SocketType.h"
#include "ClientStruct.h"
#include "SocketServerCallback.h"
#include "ClientStructSet.h"

class socrates::SocketServer {
	private:
		static const int MAX_CLIENTS = 100;

		typedef struct {
			int socket;
			bool listenning;
			socrates::ClientStructSet clientList;
			socrates::SocketServerCallback *callbacks;
			unsigned int maxClients;
		} listen_thread_t;

		struct sockaddr_in m_serverAddr;
		listen_thread_t m_threadArgs;
		bool m_isValid;

		// Threads
		pthread_t m_listenThread;
		static void *ListenRoutine(void *args);
	public:
		static const in_addr_t AllInterfaces = INADDR_ANY;
		static const in_addr_t Localhost = INADDR_LOOPBACK;

		SocketServer(in_addr_t listenTo, uint16_t port, socrates::SocketFamily family, socrates::SocketType type);
		~SocketServer();

		bool StartListenning(void);
		void StopListenning(void);
		bool IsListenning(void);

		void DefineMaxClients(unsigned int clients);
		void SetupCallbacks(socrates::SocketServerCallback &callback);

		void DisconnectClient(socrates::ClientStruct *client);
		socrates::ClientStruct *GetClientIfPresent(struct in_addr addr);
		socrates::ClientStructSet *GetConnectedClients(void);
};

#endif /* SOCKETSERVER_H_ */
