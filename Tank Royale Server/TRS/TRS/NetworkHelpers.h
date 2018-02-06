#pragma once
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <process.h>
#include <string.h>
#include <map>
#include <stdio.h>
#include <tchar.h>

class NetworkHelpers
{
public:
	static int sendMessage(SOCKET curSocket, char * message, int messageSize) {
		return send(curSocket, message, messageSize, 0);
	}

	static int receiveMessage(SOCKET curSocket, char * buffer, int bufSize) {
		return recv(curSocket, buffer, bufSize, 0);
	}

};


enum DataType {

	INIT_CONNECTION = 0,

	ACTION_EVENT = 1,

	// add timer ping, start turn, end turn
};

struct Data {

	unsigned int packet_type;

	void serialize(char * data) {
		memcpy(data, this, sizeof(Data));
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Data));
	}
};