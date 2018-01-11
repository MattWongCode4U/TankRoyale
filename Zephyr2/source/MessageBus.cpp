#include "MessageBus.h"


MessageBus::MessageBus() {
}


MessageBus::~MessageBus() {
}

void MessageBus::postMessage(Msg* m, System* sender) {
	// send the message to every system so that they can all see it

	for (System* s : systemList) {
		if (sender != s) {
			s->addToMsgQ(m);
		}
		
	}
}

// Used to remove a system from the list
void MessageBus::addSystem(System* s) {
	systemList.push_back(s);
}
