// TBGServer.cpp : Defines the entry point for the console application.
//#include "stdafx.h"
#include "Main.h"


Lobby * server;

static unsigned int client_id;
static int playerNum = 4;
static int playersReady = 0;
std::vector<std::pair<int, std::vector<std::string>>> turnInfo;

void serverLoop()
{
	while (true)
	{
		server->update();
	}
}

int main()
{
	std::cout << "Starting Server\n";
	
	server = new Lobby();

	std::cout << "Server Ready\n";

	serverLoop();

	std::cout << "\nSERVER TERMINATED\n";
	system("pause");
	return 0;
}

