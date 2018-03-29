#include "ClientStruct.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;
using namespace net;

pthread_mutex_t ClientStruct::RECEIVED_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ClientStruct::DISCONNECTED_MUTEX = PTHREAD_MUTEX_INITIALIZER;

void *ClientStruct::RecvRoutine(void *args) {
	ssize_t msgSize = -1;
	thread_args_t *arguments = (thread_args_t *)args;

	while (arguments->connected) {
		msgSize = recv(arguments->clientSocket, arguments->buffer, BUFFER_SIZE, 0);
		if (msgSize >= 0) {
			if (arguments->callback != NULL) {
				pthread_mutex_lock(&ClientStruct::RECEIVED_MUTEX);
				arguments->callback->ReceivedMessage(arguments->instance, arguments->buffer, msgSize);
				pthread_mutex_unlock(&ClientStruct::RECEIVED_MUTEX);
			}
		} else {
			arguments->connected = false;
			arguments->callback->DisconnectedClient(arguments->instance);
		}
	}

	return NULL;
}

ClientStruct::ClientStruct(int clientSocket, struct sockaddr_in addr, net::SocketServerCallback &callback) {
	m_threadArgs.addr = addr;
	m_threadArgs.clientSocket = clientSocket;
	m_threadArgs.callback = &callback;
	m_threadArgs.instance = this;

//	int errorSize = sizeof(m_threadArgs.error);
//	getsockopt(m_threadArgs.clientSocket, SOL_SOCKET, SO_ERROR, &m_threadArgs.error, (socklen_t *)&errorSize);

	int result = pthread_create(&m_recvThread, NULL, ClientStruct::RecvRoutine, (void *)&m_threadArgs);
	if (result != 0)
		m_threadArgs.connected = false;
	else
		m_threadArgs.connected = true;


}

ClientStruct::~ClientStruct(void) {
	m_threadArgs.connected = false;
	close(m_threadArgs.clientSocket);

	pthread_join(m_recvThread, NULL);
}

void ClientStruct::Close(void) {
	m_threadArgs.connected = false;
	close(m_threadArgs.clientSocket);

	pthread_join(m_recvThread, NULL);

	pthread_mutex_lock(&ClientStruct::DISCONNECTED_MUTEX);
	m_threadArgs.callback->DisconnectedClient(m_threadArgs.instance);
	pthread_mutex_unlock(&ClientStruct::DISCONNECTED_MUTEX);
}

bool ClientStruct::Connected(void) {
	return m_threadArgs.connected;
}

struct sockaddr_in *ClientStruct::GetAddress(void) {
	return &m_threadArgs.addr;
}

int ClientStruct::SendMessage(const uint8_t *msg, uint16_t length) {
	if (m_threadArgs.connected) { // TODO: handle broken pipe
		return write(m_threadArgs.clientSocket, msg, length);
	} else {
		return -1;
	}
}

int ClientStruct::SendMessage(string msg) {
	int length = msg.length();
	return SendMessage((const uint8_t *)msg.c_str(), length);
}
