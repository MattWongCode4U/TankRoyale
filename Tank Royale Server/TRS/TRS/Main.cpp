// TBGServer.cpp : Defines the entry point for the console application.
//#include "stdafx.h"
#include "Main.h"


int main()
{
	std::cout << "Starting Server\n";
	
	Server* server = new Server();

	std::cout << "Server Ready\n";

	// potential for console stuff to be done here if we shift the stuff in the
	// while loop to a new thread
	while (true)
	{
		server->update();
	}

	system("pause");
	return 0;
}

