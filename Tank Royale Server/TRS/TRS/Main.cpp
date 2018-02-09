// TBGServer.cpp : Defines the entry point for the console application.
//#include "stdafx.h"
#include "Main.h"


Lobby * server;

static unsigned int client_id;
static int playerNum = 1;
static int playersReady = 0;


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

	return 0;
}

