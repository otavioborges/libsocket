#include "ClientStruct.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

using namespace std;
using namespace net;

pthread_mutex_t ClientStruct::RECEIVED_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ClientStruct::SEND_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ClientStruct::DISCONNECTED_MUTEX = PTHREAD_MUTEX_INITIALIZER;

void *ClientStruct::RecvRoutine(void *args) {
	ssize_t msgSize = -1;
	thread_args_t *arguments = (thread_args_t *)args;

	while (arguments->connected) {
		msgSize = recv(arguments->clientSocket, arguments->buffer, BUFFER_SIZE, 0);
		if (msgSize > 0) {
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

void *ClientStruct::SendRoutine(void *args) {
	uint16_t length;
	uint8_t *msg;
	thread_args_t *arguments = (thread_args_t *)args;

	while (arguments->connected) {

		pthread_mutex_lock(&SEND_MUTEX);
		if (arguments->bufferSendMsgs.size() <= 0) {
			pthread_mutex_unlock(&SEND_MUTEX);
			usleep(10000);
			continue;
		}
		length = arguments->bufferSendMsgs[0].first;
		msg = arguments->bufferSendMsgs[0].second;
		pthread_mutex_unlock(&SEND_MUTEX);

		int result = write(arguments->clientSocket, msg, length);
		if (result < 0) {
			arguments->connected = false;
			arguments->callback->DisconnectedClient(arguments->instance);

			break; // no need trying to send the rest
		} else {
			pthread_mutex_lock(&SEND_MUTEX);
			arguments->bufferSendMsgs.erase(arguments->bufferSendMsgs.begin());
			pthread_mutex_unlock(&SEND_MUTEX);

			arguments->callback->SendCallback(arguments->instance, result);
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

	result = pthread_create(&m_sendThread, NULL, ClientStruct::SendRoutine, (void *)&m_threadArgs);
	if (result != 0) {
		m_threadArgs.connected = false;
	}
}

ClientStruct::~ClientStruct(void) {
	m_threadArgs.connected = false;
	close(m_threadArgs.clientSocket);

	pthread_mutex_lock(&SEND_MUTEX);
	vector<pair<uint16_t, uint8_t *>>::iterator it;
	for (it = m_threadArgs.bufferSendMsgs.begin(); it != m_threadArgs.bufferSendMsgs.end(); ++it) {
		delete[] (*it).second;
		m_threadArgs.bufferSendMsgs.erase(it);
	}
	pthread_mutex_unlock(&SEND_MUTEX);
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

int ClientStruct::SendMessage(const uint8_t *msg, uint16_t length, bool blocking) {
	if (m_threadArgs.connected) {
		if (blocking) {
			return write(m_threadArgs.clientSocket, msg, length);
		} else {
			pthread_mutex_lock(&SEND_MUTEX);
			uint8_t *msgBuffer = new uint8_t[length];
			memcpy(msgBuffer, msg, length);
			m_threadArgs.bufferSendMsgs.push_back(make_pair(length, msgBuffer));
			pthread_mutex_unlock(&SEND_MUTEX);
			return 0;
		}
	} else {
		return -1;
	}
}

int ClientStruct::SendMessage(string msg, bool blocking) {
	int length = msg.length();
	return SendMessage((const uint8_t *)msg.c_str(), length, blocking);
}

int ClientStruct::PendingMessages(void) {
	return m_threadArgs.bufferSendMsgs.size();
}
