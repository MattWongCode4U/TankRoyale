#pragma once
#include "NetworkHelpers.h"
#include "Server.h"
#define MAX_PACKET_SIZE 1000000

extern unsigned int client_id;


class Lobby
{

public:

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

			client_id++;
		}
		receiveFromClients();

	}

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

			int i = 0;
			while (i < (unsigned int)data_length)
			{
				packet.deserialize(&(network_data[i]));
				i += sizeof(Data);

				switch (packet.packet_type) {

				case INIT_CONNECTION:

					printf("server received init packet from client\n");

					sendActionPackets();

					break;

				case ACTION_EVENT:

					printf("server received action event packet from client\n");

					sendActionPackets();

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

};