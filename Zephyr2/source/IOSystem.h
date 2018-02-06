#pragma once
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "System.h"
#include "RenderSystem.h"

class IOSystem : public System {
public:
	IOSystem(MessageBus* mbus);
	~IOSystem();

	void checkKeyPresses();
	void startSystemLoop();
	void handleMessage(Msg * msg);

	Msg* m;
	
	const int timeFrame = 20;

	std::map<std::string, clock_t> keyspressed;

	const int timebetweenPresses = 150;
	bool focus = true;
private:
	
};
