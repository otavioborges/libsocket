#ifndef CLIENTSTRUCTSET_H_
#define CLIENTSTRUCTSET_H_

#include <set>
#include <utility>
#include <netinet/in.h>
#include "socrates.h"
#include "ClientStruct.h"

class socrates::ClientStructSet : public std::set<socrates::ClientStruct *> {
	public:
		typedef typename std::set<socrates::ClientStruct *>::iterator iterator;

		ClientStructSet() : std::set<socrates::ClientStruct *>() {};
		iterator find(const struct sockaddr_in& val) const;
};

#endif /* CLIENTSTRUCTSET_H_ */