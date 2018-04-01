#include "ClientStructSet.h"

using namespace socrates;

ClientStructSet::iterator ClientStructSet::find(const struct sockaddr_in& val) const {
	iterator it = this->begin();

	while (it != this->end()) {
		struct sockaddr_in *targetAddr = (*it)->GetAddress();
		if (val.sin_addr.s_addr == targetAddr->sin_addr.s_addr)
			return it;
	}

	return it; // equals to end
}