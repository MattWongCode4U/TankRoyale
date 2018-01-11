#pragma once
#include "Msg.h"
#include "System.h"
#include <list>
#include <iostream>

class System;

class MessageBus {
public:
	MessageBus();
	~MessageBus();

	void postMessage(Msg* m, System* sender);
	void addSystem(System* s);
private:
	std::list<System*> systemList;
};

