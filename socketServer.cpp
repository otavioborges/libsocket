#include <string.h>
#include <time.h>
#include <unistd.h>
#include "socketServer.h"
using namespace net;
using namespace std;

typedef vector<ClientStruct *>::iterator clientListIterator;

void *SocketServer::ListenRoutine(void *args) {
	int result = -1;
	int clientSocket = -1;
	struct sockaddr_in clientAddr;
	int clientAddrLength = 0;
	listen_thread_t *arguments = (listen_thread_t *)args;

	while ((arguments->listenning) && (arguments->clientList.size() < arguments->maxClients)) {
		result = listen(arguments->socket, 1);
		if (result >= 0) {
			clientAddrLength = sizeof(clientAddr);
			clientSocket = accept(arguments->socket, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrLength);
			if (clientSocket >= 0) {
				arguments->clientList.push_back(new ClientStruct(clientSocket, clientAddr, *arguments->callbacks));
				arguments->callbacks->ConnectedClient(clientAddr);
			}
		}
	}

	return NULL;
}

SocketServer::SocketServer(in_addr_t listenTo, uint16_t port, SocketFamily family, SocketType type) {
	m_threadArgs.maxClients = MAX_CLIENTS;
	m_threadArgs.socket = socket((int)family, (int)type, 0);
	if (m_threadArgs.socket < 0) {
		m_isValid = false;
		return;
	}

	memset(&m_serverAddr, 0, sizeof(struct sockaddr_in));
	m_serverAddr.sin_family = (int)family;
	m_serverAddr.sin_addr.s_addr = listenTo;
	m_serverAddr.sin_port = htons(port);

	m_threadArgs.callbacks = NULL;

	int result = bind(m_threadArgs.socket, (struct sockaddr *)&m_serverAddr, sizeof(struct sockaddr_in));
	if (result < 0)
		m_isValid = false;
	else
		m_isValid = true;

	// setup socket options
//	struct timeval timeout;
//	timeout.tv_sec = 10;
//	timeout.tv_usec = 0;
//
//	setsockopt(m_socket, SOL_SOCKET, SO_BINDTODEVICE)
}


SocketServer::~SocketServer() {
	clientListIterator it;

	for (it = m_threadArgs.clientList.end(); it != m_threadArgs.clientList.begin(); --it) {
		delete (*it);
		m_threadArgs.clientList.erase(it);
	}

	shutdown(m_threadArgs.socket, SHUT_RDWR);
	close(m_threadArgs.socket);
}

bool SocketServer::StartListenning(void) {
	if (!m_isValid)
		return false;

	m_threadArgs.listenning = true;

	int result = pthread_create(&m_listenThread, NULL, SocketServer::ListenRoutine, (void *)&m_threadArgs);
	if (result != 0) {
		m_threadArgs.listenning = false;
		m_isValid = false;

		return false;
	}

	return true;
}

void SocketServer::StopListenning(void) {
	m_threadArgs.listenning = false;
}

bool SocketServer::IsListenning(void) {
	return m_threadArgs.listenning;
}

void SocketServer::DefineMaxClients(unsigned int clients) {
	m_threadArgs.maxClients = clients;

	if (m_threadArgs.clientList.size() > m_threadArgs.maxClients) {
		clientListIterator it = m_threadArgs.clientList.end();

		while (m_threadArgs.clientList.size() > m_threadArgs.maxClients) {
			it--;

			delete (*it);
			m_threadArgs.clientList.erase(it);
		}
	}
}

void SocketServer::SetupCallbacks(SocketServerCallback &callback) {
	m_threadArgs.callbacks = &callback;
}

vector<ClientStruct *> SocketServer::GetConnectedClients(void) {
	return m_threadArgs.clientList;
}