#pragma once
#include "NetworkHelpers.h"
#include "Server.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <vector>
#define MAX_PACKET_SIZE sizeof(Data)

extern unsigned int client_id;
extern int playerNum;
extern int playersReady;


class Lobby
{
public:
	bool ingame = false;
	std::vector<int> playingPlayers;
	int readyPlayerNum = 0;
	int gameStartTime = 0;
	int timerPacketsSent = 0;
	bool animationsInProgress = false;
	int doneanimations = 0;

	std::string action1 = "";
	std::string action2 = "";
	std::string action3 = "";
	std::string action4 = "";
	std::string defaultActionString = "IDLE, 0, 0";

	/*turnInfo =
		playerID + "," + playerTurnAction[0] + "," + playerTurnTargetX[0] + "," + playerTurnTargetY[0] + "]player2, MOVE, 100,100]player3, MOVE, -20,110]player4, MOVE, -120,0]\n" +
		playerID + "," + playerTurnAction[1] + "," + playerTurnTargetX[1] + "," + playerTurnTargetY[1] + "]player2, MOVE, 100,80]player3, MOVE, -20,100]player4, MOVE, -100,-10]\n" +
		playerID + "," + playerTurnAction[2] + "," + playerTurnTargetX[2] + "," + playerTurnTargetY[2] + "]player2, MOVE, 100,60]player3, MOVE, -20,90]player4, MOVE, -80,-30]\n" +
		playerID + "," + playerTurnAction[3] + "," + playerTurnTargetX[3] + "," + playerTurnTargetY[3] + "]player2, MOVE, 100,40]player3, MOVE, -20,80]player4, MOVE, -60,-50]";*/

	Lobby(void) {
		// id's to assign clients for our table
		client_id = 0;

		// set up the server network to listen 
		network = new Server();
	}

	~Lobby(void) {}

	void update() {

		// get new clients
		if (network->acceptNewClient(client_id))
		{
			printf("client %d has been connected to the server\n", client_id);
			sendClientID(client_id);
			client_id++;
		}

		// read from clients
		receiveFromClients();

		if (ingame) {
			// clock
			// send current time
			if (!animationsInProgress) {
				sendTimerPackets();
				Sleep(1000);
			} else {
				// wait for animations to complete
				if (doneanimations == playerNum) {
					animationsInProgress = false;
					doneanimations = 0; // might have issues with dead players not sending.
				}
			}
		}
	}

	void receiveFromClients()
	{
		Data packet;

		// go through all clients
		std::map<unsigned int, SOCKET>::iterator iter;

		for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
		{
			int data_length = network->receiveData(iter->first, network_data);

			if (data_length <= 0)
			{
				//no data recieved
				continue;
			}
			
			std::cout << "|" << network_data << "|\n";

			int i = 0;
			while (i < (unsigned int)data_length)
			{
				packet.deserialize(&(network_data[i]));
				i += sizeof(Data);

				switch (packet.packet_type) {
				case PLAYER_ACTION:
					// aggreagate turn info str
					break;
				case ANIMATIONS_COMPLETE:
					printf("server received animationComplete message");
					doneanimations++;
					break;
				case INIT_CONNECTION:
					printf("server received init packet from client\n");
					// sendActionPackets();
					break;
				case ACTION_EVENT:
					printf("server received action event packet from client\n");
					// std::cout << packet.actualData;
					// sendActionPackets();
										
					if (readyPlayerNum == playerNum) {
						// remove enough players to start the game 
						for (auto x : network->readyPlayers) {
							if (x.second == true) {
								// this player is ready to start
								for (int i = 0; i < network->playersInQueue.size(); i++) {
									// is this the player that is ready
									if (x.first == network->playersInQueue.at(i)) {
										playingPlayers.push_back(network->playersInQueue.at(i));
										network->playersInQueue.erase(network->playersInQueue.begin() + i);
										break; // dont need to search, id is unique
									}
								}
							}
						}

						network->readyPlayers.clear();

						ingame = true;
						gameStartTime = clock();
						readyPlayerNum = 0;
						sendStartGamePackets();
						sendStartTurnPackets();
					} else {
						// update and set that this guy is ready
						if (network->readyPlayers.count(iter->first) == 0) {
							// new ready
							readyPlayerNum++;
							network->readyPlayers[iter->first] = true;
						}
					}

					break;
				default:

					printf("error in packet types\n");

					break;
				}
			}
		}
	}

private:


	// The ServerNetwork object 
	Server* network;

	char network_data[MAX_PACKET_SIZE];

	void sendActionPackets()
	{
		// send action packet
		const unsigned int packet_size = sizeof(Data);
		char packet_data[packet_size];

		Data packet;
		packet.packet_type = ACTION_EVENT;

		packet.serialize(packet_data);

		network->sendToAll(packet_data, packet_size);
	}

	void sendStartGamePackets()
	{
		char packet_data[MAX_PACKET_SIZE];

		Data packet;
		packet.packet_type = GAME_START;

		// send the ID's in order
		std::ostringstream oss;
		
		for (auto i : playingPlayers) {
			oss << i << ",";
		}

		std::string s = oss.str();
		packet.setData(s.c_str());
		packet.serialize(packet_data);

		network->sendToPlaying(packet_data, MAX_PACKET_SIZE, playingPlayers);
	}

	void sendStartTurnPackets()
	{
		char packet_data[MAX_PACKET_SIZE];

		Data packet;
		packet.packet_type = TURN_START;

		packet.serialize(packet_data);

		network->sendToPlaying(packet_data, MAX_PACKET_SIZE, playingPlayers);
	}

	void sendTimerPackets()
	{
		char packet_data[MAX_PACKET_SIZE];

		Data packet;
		packet.packet_type = TIMER_PING;
		// add clock time
		packet.serialize(packet_data);


		network->sendToPlaying(packet_data, MAX_PACKET_SIZE, playingPlayers);
		timerPacketsSent++;

		if (timerPacketsSent == 30) {
			sendTurnInformation();
			timerPacketsSent = 0;
		}
	}

	// TODO
	void sendTurnInformation() {
		char packet_data[MAX_PACKET_SIZE];

		Data packet;
		packet.packet_type = TURN_OVER;
		// fill in turn information here


		packet.serialize(packet_data);

		network->sendToPlaying(packet_data, MAX_PACKET_SIZE, playingPlayers);
		animationsInProgress = true;
	}

	void sendClientID(int id) {
		char packet_data[MAX_PACKET_SIZE];

		Data packet;
		packet.packet_type = INIT_CONNECTION;
		// this is your id
		std::ostringstream oss;
		oss << id;
		std::string s = oss.str();
		packet.setData(s.c_str());
		packet.serialize(packet_data);

		network->sendToPlaying(packet_data, MAX_PACKET_SIZE, playingPlayers);
	}

	// TODO
	void resetTurnInformation() {
		for (int i = 0; i < 4; i++) {
			
		}
	}

};