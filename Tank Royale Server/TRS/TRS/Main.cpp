// TBGServer.cpp : Defines the entry point for the console application.
//#include "stdafx.h"
#include "Main.h"


Lobby * server;

static unsigned int client_id;

void serverLoop()
{
	while (true)
	{
		server->update();
	}
}

int main()
{
	server = new Lobby();

	serverLoop();

	return 0;
}

