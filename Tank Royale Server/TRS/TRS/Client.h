#pragma once
#include "NetworkHelpers.h"

class Client {
public:
	
	Client(int id, SOCKET ss);

	~Client() {}

	int clientID;
	SOCKET s;
	
	bool disconnected = false;

	std::string classType;

	// sendToSocket();
};