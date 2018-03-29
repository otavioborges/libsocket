#ifndef CLIENTSTRUCTSET_H_
#define CLIENTSTRUCTSET_H_

#include <set>
#include <utility>
#include <netinet/in.h>
#include "net.h"
#include "ClientStruct.h"

class net::ClientStructSet : public std::set<net::ClientStruct *> {
	public:
		typedef typename std::set<net::ClientStruct *>::iterator iterator;

		ClientStructSet() : std::set<net::ClientStruct *>() {};
		iterator find(const struct sockaddr_in& val) const;
};

#endif /* CLIENTSTRUCTSET_H_ */