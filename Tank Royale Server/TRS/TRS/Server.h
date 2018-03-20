#pragma once
#include <vector>
#include "Lobby.h"
#include "Client.h"
#include "NetworkHelpers.h"
#include <vector>
#define DEFAULT_PORT "9876" 
#define MAX_PACKET_SIZE sizeof(Data)

class Lobby;

class Server
{

public:
	unsigned int client_id = 0;
	int playerNum = 4;
	int playersReady = 0;

	std::vector<int> playersInQueue;
	std::map<int, bool> readyPlayers;

	Server(void);

	~Server(void) {}

	// Socket to listen for new connections
	SOCKET ListenSocket;

	// Socket to give to the clients
	SOCKET ClientSocket;

	// for error checking return values
	int iResult;

	// table to keep track of each client's socket - master copy
	std::map<unsigned int, SOCKET> sessions;

	bool acceptNewClient(unsigned int & id);

	int receiveData(unsigned int client_id, char * recvbuf);

	void sendToAll(char * packets, int totalSize);
	void sendToSpecific(char* packets, int totalSize, int id);
	void sendToPlaying(char* packets, int totalSize, std::vector<int> playing);


	std::vector<Lobby*> lobbies;
	std::vector<Client*> clients;
	void update();

	void sendToSocket(char* packets, int totalSize, SOCKET s);
	int receiveDataS(SOCKET& s, char * recvbuf);

};