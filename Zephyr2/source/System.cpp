#include "System.h"


System::System(MessageBus* mbus) {
	msgBus = mbus;
}


System::~System() {
}

void System::handleMessage(Msg *msg) {
	switch (msg->type) {
	case EXIT_GAME: 
		alive = false;
		break;
	default:
		break;
	}

}

void System::addToMsgQ(Msg *msg) {
	mtx.lock();
	msgQ.push(msg);
	mtx.unlock();
}

void System::handleMsgQ() {
	mtx.lock();
	while (!msgQ.empty()) {
		handleMessage(msgQ.front());
		msgQ.pop();	
	}
	mtx.unlock();
}