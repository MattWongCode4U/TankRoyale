#pragma once
#include "Msg.h"
#include "MessageBus.h"
#include <queue>
#include <mutex>
#include <windows.h>
#include <string>  

class MessageBus;

class System {
public:
	System(MessageBus* mbus);
	~System();

	virtual void handleMessage(Msg *msg);
	void addToMsgQ(Msg *msg);
	
	volatile bool alive = true;
protected:
	MessageBus* msgBus;
	std::queue<Msg*> msgQ;
	void handleMsgQ();
	std::mutex mtx;

private:
	
	
};

