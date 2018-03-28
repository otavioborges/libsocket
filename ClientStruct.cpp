#include "ClientStruct.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;
using namespace net;

pthread_mutex_t ClientStruct::CALLBACK_MUTEX = PTHREAD_MUTEX_INITIALIZER;

void *ClientStruct::RecvRoutine(void *args) {
	ssize_t msgSize = -1;
	thread_args_t *arguments = (thread_args_t *)args;

	while (arguments->running) {
		msgSize = recv(arguments->clientSocket, arguments->buffer, BUFFER_SIZE, 0);
		if (msgSize >= 0) {
			if (arguments->callback != NULL) {
				pthread_mutex_lock(&ClientStruct::CALLBACK_MUTEX);
				arguments->callback->ReceivedMessage(&arguments->addr, arguments->buffer, msgSize);
				pthread_mutex_unlock(&ClientStruct::CALLBACK_MUTEX);
			}
		}
	}

	return NULL;
}

ClientStruct::ClientStruct(int clientSocket, struct sockaddr_in addr, net::SocketServerCallback &callback) {
	m_threadArgs.addr = addr;
	m_threadArgs.clientSocket = clientSocket;
	m_threadArgs.callback = &callback;

	int result = pthread_create(&m_recvThread, NULL, ClientStruct::RecvRoutine, (void *)&m_threadArgs);
	if (result != 0)
		m_threadArgs.running = false;
	else
		m_threadArgs.running = true;


}

ClientStruct::~ClientStruct() {
	shutdown(m_threadArgs.clientSocket, SHUT_RDWR);
	close(m_threadArgs.clientSocket);

	m_threadArgs.running = false;
	pthread_join(m_recvThread, NULL);
	pthread_mutex_destroy(&ClientStruct::CALLBACK_MUTEX);
}

bool ClientStruct::Running(void) {
	return m_threadArgs.running;
}

int ClientStruct::SendMessage(const uint8_t *msg, uint16_t length) {
	if (m_threadArgs.running) { // TODO: handle broken pipe
		return write(m_threadArgs.clientSocket, msg, length);
	} else {
		return -1;
	}
}

int ClientStruct::SendMessage(string msg) {
	int length = msg.length();
	return SendMessage((const uint8_t *)msg.c_str(), length);
}
