#pragma once
#include "NetworkHelpers.h"
#include "Server.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <vector>
#define MAX_PACKET_SIZE sizeof(Data)
#define ACTIONS_PER_TURN 4

// this is basically the "in game" that deals with only the players connected to a game
class Lobby
{
public:
	bool ingame = false;
	bool full = false;
	int gameStartTime = 0;
	int timerPacketsSent = 0;
	bool animationsInProgress = false;
	int doneanimations = 0;

	std::string defaultActionString = "0,2,0,0";

	Lobby(Server* n);

	~Lobby(void) {}

	void update();

	void receiveFromClients();
	
	// The ServerNetwork object 
	Server* network;
	std::vector<Client*> players;


	int connectedPlayers = 0;

	char network_data[MAX_PACKET_SIZE];

	void sendActionPackets();

	void sendConnID(Client* c);

	void sendStartGamePackets();

	void sendStartTurnPackets();

	void sendTimerPackets();

	void sendTurnInformation();

	void initializeTurnInformation();

	void resetTurnInformation();

	bool validateTurnsComplete();

	void sendToPlayers(char* packets, int totalSize);

	void cleanupDisconnected();
	int actionsReceived = 0;
	std::vector<std::pair<int, std::vector<std::string>>> turnInfo;

	void handlePlayerAction(Data p, int id);
	void sendToClient(Client*& c, char* packets, int totalSize);
	void handleActionEvent();
};