#pragma once
#include "System.h"

class NetworkSystem : public System
{
public:

	// use config file to set this later
	// for testing we'll leave both echo mode to true and serverIP to empty (which should also be set via confgig file)
	bool echoMode = true;
	std::string serverIP = "";

	NetworkSystem(MessageBus* mbus);
	~NetworkSystem();

	void startSystemLoop();
	void handleMessage(Msg *msg);
	void aggregateTurnInfo(Msg *m);
	void broadcastTurnInfo();

	Msg* m;
	
	std::string playerTurnAction[4];
	std::string playerTurnTarget[4];
	std::string playerTurnTargetX[4];
	std::string playerTurnTargetY[4];

	const int timeFrame = 20;

	int actionCounter = 0;
	// temp set here, should be set in config, and the gameysstem needs its own too probably
	int maxActions = 4;

	std::string playerID = "player1"; // needs to be set in config properly 

	// 30 secs - used during testing, server uses timer irl
	const int turnTimer = 30000;
};
