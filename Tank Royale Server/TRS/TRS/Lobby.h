#pragma once
#include "NetworkHelpers.h"
#include "Server.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <vector>
#define MAX_PACKET_SIZE sizeof(Data)
#define ACTIONS_PER_TURN 4

extern unsigned int client_id;
extern int playerNum;
extern int playersReady;
extern std::vector<std::pair<int, std::vector<std::string>>> turnInfo;

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

	std::string defaultActionString = "2,0,0";

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
					sendStartTurnPackets();
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
			
			// std::cout << "|" << network_data << "|\n";

			int i = 0;
			while (i < (unsigned int)data_length)
			{
				packet.deserialize(&(network_data[i]));
				i += sizeof(Data);

				switch (packet.packet_type) {
				case PLAYER_ACTION:
				{
					bool aggregated = false;
					// aggreagate turn info str
					for (auto& t : turnInfo) {
						if (!aggregated) {
							// first find te right pair using the id
							if (t.first == iter->first) {
								for (auto& s : t.second) {
									// next, find the first string that's still default
									if (s == defaultActionString) {
										// modify it
										s = packet.actualData;
										printf("PLAYER ACTION ACTUAL DATA: ");
										printf(s.c_str());
										printf("\n");
										aggregated = true;
										break;// done
									}
								}
							}
						}
					}

					if (validateTurnsComplete()) {
						printf("ALL TURNS COMPLETE RETURNED TRUE\n");
						// this means we have all the required messages
						sendTurnInformation();
						timerPacketsSent = 0;
					}
				}
					break;
				case ANIMATIONS_COMPLETE:
					printf("server received animationComplete message");
					doneanimations++;
					break;
				case INIT_CONNECTION:
					printf("server received init packet from client\n");
					sendConnID(iter->first);
					break;
				case ACTION_EVENT:
					printf("server received action event packet from client\n");
					// std::cout << packet.actualData;
					// sendActionPackets();
					{
						printf("not enough players ready\n");
						// update and set that this guy is ready
						if (network->readyPlayers.count(iter->first) == 0) {
							// new ready
							std::ostringstream os;
							os << iter->first;
							printf("adding a player ");
							printf(os.str().c_str());
							printf("\n");

							readyPlayerNum++;
							network->readyPlayers[iter->first] = true;
						}

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
							printf("starting a game\n");
							network->readyPlayers.clear();

							ingame = true;
							gameStartTime = clock();
							readyPlayerNum = 0;
							sendStartGamePackets();
							sendStartTurnPackets();
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
	// this is disgusting but i'm doing it anyways
	// int would be the id, add moves to inner vector position, pad with empty data if it doesn't exist
	

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

	void sendConnID(int id)
	{
		// send action packet
		const unsigned int packet_size = sizeof(Data);
		char packet_data[packet_size];

		Data packet;
		packet.packet_type = INIT_CONNECTION;

		std::ostringstream oss;
		oss << id;

		packet.setData(oss.str().c_str());
		packet.serialize(packet_data);

		network->sendToSpecific(packet_data, packet_size, id);
	}

	void sendStartGamePackets()
	{
		// clear the previous turn info if any exists
		turnInfo.clear();

		char packet_data[MAX_PACKET_SIZE];

		Data packet;
		packet.packet_type = GAME_START;

		// send the ID's in order
		std::ostringstream oss;
		
		for (auto i : playingPlayers) {
			oss << i << ",";

			// while we're at it, start to initalize the turn information for the game
			std::vector<std::string> vs;

			//std::pair<int, std::vector<std::string>> p(i, vs);
			
			turnInfo.push_back(std::make_pair(i, vs));
		}

		initializeTurnInformation();

		std::string s = oss.str();
		packet.setData(s.c_str());
		packet.serialize(packet_data);

		network->sendToPlaying(packet_data, MAX_PACKET_SIZE, playingPlayers);
	}

	void sendStartTurnPackets()
	{
		resetTurnInformation();

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

	void sendTurnInformation() {
		char packet_data[MAX_PACKET_SIZE];

		Data packet;
		packet.packet_type = TURN_OVER;
		// fill in turn information here

		std::ostringstream oss;
		
		for (int i = 0; i < ACTIONS_PER_TURN; i++) {
			for (auto& t : turnInfo) {
				oss << t.first << "," << t.second.at(i) << "]";
				oss << t.second.at(i) << "]";
			}
			oss << "\n";
		}

		packet.setData(oss.str().c_str());

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

	void initializeTurnInformation() {
		for (auto& t : turnInfo) {
			for (int i = 0; i < ACTIONS_PER_TURN; i++) {
				t.second.push_back(defaultActionString);
			}
		}
	}

	void resetTurnInformation() {
		for (auto& t : turnInfo) {
			for (std::string s : t.second) {
				s = defaultActionString;
			}
		}
	}

	bool validateTurnsComplete() {
		for (auto& t : turnInfo) {
			for (std::string s : t.second) {
				printf("Validating ");
				printf(s.c_str());
				printf("\n");
				if (s == defaultActionString) {
					return false;
				}
			}
		}

		return true;
	}

};