#pragma once
#include "System.h"
#include <chrono>
#include <thread>
#pragma comment (lib, "Ws2_32.lib")

class NetworkSystem : public System {
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
	std::string playerTurnTargetX[4];
	std::string playerTurnTargetY[4];

	const int timeFrame = 100;

	int actionCounter = 0;
	// temp set here, should be set in config, and the gameysstem needs its own too probably
	int maxActions = 4;

	std::string playerID = "player1"; // needs to be set in config properly 

	// 30 secs - used during testing, server uses timer irl
	const int turnTimer = 30000;

	//is the turnTimer coundown active? used in echo mode
	bool timerActive = false;
	int turnStartTime = 0;
	void startTimer();

	// for error checking function calls in Winsock library
	int iResult;

	// socket for client to connect to server
	SOCKET ConnectSocket;

	int receivePackets(char *);
	void sendActionPackets();

	char network_data[MAX_PACKET_SIZE];

	void networkUpdate();

	void sendPacket(DataType d, std::string data);
};
