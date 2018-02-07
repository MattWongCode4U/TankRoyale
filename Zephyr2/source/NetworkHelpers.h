#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <string.h>
#include <map>
#include <stdio.h>
#include <tchar.h>
#define MAX_PACKET_SIZE sizeof(Data)
#define DEFAULT_PORT "9876"  
#define BUFFER 512


class NetworkHelpers {
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

	GAME_START = 2,

	TIMER_PING = 3,

	TURN_START = 4,

	TURN_OVER = 5, // also contains turn information

	START_ANIMATIONS = 6,

	ANIMATIONS_COMPLETE = 7,

};

struct Data {

	int packet_type;
	char actualData[BUFFER];

	//void serialize(char * data) {
	//	memcpy(data, this, sizeof(Data));
	//}

	//void deserialize(char * data) {
	//	memcpy(this, data, sizeof(Data));
	//}

	void serialize(char *data)
	{
		int *q = (int*)data;
		*q = packet_type;
		q++;

		char *p = (char*)q;
		int i = 0;
		while (i < BUFFER)
		{
			*p = actualData[i];
			p++;
			i++;
		}
	}

	void deserialize(char *data)
	{
		int *q = (int*)data;
		packet_type = *q;
		q++;

		char *p = (char*)q;
		int i = 0;
		while (i < BUFFER)
		{
			actualData[i] = *p;
			p++;
			i++;
		}
	}

	void setData(const char* d) {
		strcpy_s(actualData, d);
	}
};