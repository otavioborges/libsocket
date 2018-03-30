#include "socketClient.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

using namespace std;
using namespace net;

pthread_mutex_t SocketClient::RECEIVED_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t SocketClient::SEND_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t SocketClient::DISCONNECTED_MUTEX = PTHREAD_MUTEX_INITIALIZER;

void *SocketClient::RecvRoutine(void *args) {
	ssize_t msgSize = -1;
	client_args_t *arguments = (client_args_t *)args;

	while (arguments->connected) {
		msgSize = recv(arguments->socket, arguments->buffer, BUFFER_SIZE, 0);
		if (msgSize > 0) {
			if (arguments->callback != NULL) {
				pthread_mutex_lock(&SocketClient::RECEIVED_MUTEX);
				arguments->callback->ReceivedMessage(arguments->instance, arguments->buffer, msgSize);
				pthread_mutex_unlock(&SocketClient::RECEIVED_MUTEX);
			}
		} else {
			arguments->connected = false;
			arguments->callback->Disconnected(arguments->instance);
		}
	}

	return NULL;
}

void *SocketClient::SendRoutine(void *args) {
	uint16_t length;
	uint8_t *msg;
	client_args_t *arguments = (client_args_t *)args;

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

		int result = write(arguments->socket, msg, length);
		if (result < 0) {
			arguments->connected = false;
			arguments->callback->Disconnected(arguments->instance);

			break; // no need trying to send the rest
		} else {
			pthread_mutex_lock(&SEND_MUTEX);
			arguments->bufferSendMsgs.erase(arguments->bufferSendMsgs.begin());
			pthread_mutex_unlock(&SEND_MUTEX);
			
			// TODO: skip callbacks when not defined
			arguments->callback->SendCallback(arguments->instance, result);
		}
	}

	return NULL;
}

SocketClient::SocketClient(string server, uint16_t port, net::SocketFamily family, net::SocketType type) {
	m_threadArgs.instance = this;
	m_threadArgs.connected = false;
	m_threadArgs.socket = socket((int)family, (int)type, 0);
	if (m_threadArgs.socket < 0) {
		m_isValid = false;
		return;
	}

	memset(&m_threadArgs.serverAddr, 0, sizeof(struct sockaddr_in));
	m_threadArgs.serverAddr.sin_family = (int)family;
	m_threadArgs.serverAddr.sin_port = htons(port);

	if (inet_pton((int)family, server.c_str(), &m_threadArgs.serverAddr.sin_addr) <= 0) {
		// not possible to break down addres, trying dns
		struct hostent *he;
		struct in_addr **addr_list;
		if ((he = gethostbyname(server.c_str())) == NULL) {
			m_isValid = false; // not possible to resolve address
			return;
		}

		addr_list = (struct in_addr **) he->h_addr_list;

		// get the first
		m_threadArgs.serverAddr.sin_addr = *addr_list[0];
	}

	m_isValid = true;
	m_threadArgs.callback = NULL;
}

SocketClient::SocketClient(in_addr_t server, uint16_t port, net::SocketFamily family, net::SocketType type) {
	m_threadArgs.instance = this;
	m_threadArgs.connected = false;
	m_threadArgs.socket = socket((int)family, (int)type, 0);
	if (m_threadArgs.socket < 0) {
		m_isValid = false;
		return;
	}

	memset(&m_threadArgs.serverAddr, 0, sizeof(struct sockaddr_in));
	m_threadArgs.serverAddr.sin_family = (int)family;
	m_threadArgs.serverAddr.sin_addr.s_addr = server;
	m_threadArgs.serverAddr.sin_port = htons(port);

	m_threadArgs.callback = NULL;
}

SocketClient::~SocketClient(void) {
	m_threadArgs.connected = false;
	close(m_threadArgs.socket);

	pthread_join(m_recvThread, NULL);

	pthread_mutex_lock(&SEND_MUTEX);
	vector<pair<uint16_t, uint8_t *>>::iterator it;
	for (it = m_threadArgs.bufferSendMsgs.begin(); it != m_threadArgs.bufferSendMsgs.end(); ++it) {
		delete[](*it).second;
		m_threadArgs.bufferSendMsgs.erase(it);
	}
	pthread_mutex_unlock(&SEND_MUTEX);
	pthread_join(m_sendThread, NULL);
}

bool SocketClient::Connect(void) {
	if (m_threadArgs.connected)
		return true;

	int result = connect(m_threadArgs.socket, (struct sockaddr *)&m_threadArgs.serverAddr, sizeof(m_threadArgs.serverAddr));
	if (result < 0) {
		m_threadArgs.connected = false;
		return false;
	} else {
		m_threadArgs.connected = true;
	}

	result = pthread_create(&m_recvThread, NULL, SocketClient::RecvRoutine , (void *)&m_threadArgs);
	if (result != 0) {
		m_threadArgs.connected = false;
		close(m_threadArgs.socket);
	}

	result = pthread_create(&m_sendThread, NULL, SocketClient::SendRoutine, (void *)&m_threadArgs);
	if (result != 0) {
		m_threadArgs.connected = false;
		close(m_threadArgs.socket);
	}

	return m_threadArgs.connected;
}

void SocketClient::Close(void) {
	m_threadArgs.connected = false;
	close(m_threadArgs.socket);

	pthread_join(m_recvThread, NULL);

	pthread_mutex_lock(&SEND_MUTEX);
	vector<pair<uint16_t, uint8_t *>>::iterator it;
	for (it = m_threadArgs.bufferSendMsgs.begin(); it != m_threadArgs.bufferSendMsgs.end(); ++it) {
		delete[](*it).second;
		m_threadArgs.bufferSendMsgs.erase(it);
	}
	pthread_mutex_unlock(&SEND_MUTEX);
	pthread_join(m_sendThread, NULL);

	pthread_mutex_lock(&SocketClient::DISCONNECTED_MUTEX);
	m_threadArgs.callback->Disconnected(m_threadArgs.instance);
	pthread_mutex_unlock(&SocketClient::DISCONNECTED_MUTEX);
}

bool SocketClient::Connected(void) {
	return m_threadArgs.connected;
}

void SocketClient::SetupCallbacks(net::SocketClientCallback &callback) {
	m_threadArgs.callback = &callback;
}

struct sockaddr_in *SocketClient::GetServerAddress(void) {
	return &m_threadArgs.serverAddr;
}

int SocketClient::SendMessage(const uint8_t *msg, uint16_t length, bool blocking) {
	if (m_threadArgs.connected) {
		if (blocking) {
			return write(m_threadArgs.socket, msg, length);
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

int SocketClient::SendMessage(string msg, bool blocking) {
	int length = msg.length();
	return SendMessage((const uint8_t *)msg.c_str(), length, blocking);
}

int SocketClient::PendingMessages(void) {
	return m_threadArgs.bufferSendMsgs.size();
}
