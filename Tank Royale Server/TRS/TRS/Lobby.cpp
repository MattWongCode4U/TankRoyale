#include "Lobby.h"

Lobby::Lobby(Server* n) {
	// set up the server network to listen 
	network = n;
	connectedPlayers = 4;
	ingame = false;
	initializeTurnInformation();
}

Lobby::~Lobby(void) {

}

void Lobby::update() {
	// check clients for info
	receiveFromClients();

	if (ingame) {
		// clock
		// send current time
		if (!animationsInProgress) {
			sendTimerPackets();
			Sleep(1000);
		} else {
			// wait for animations to complete
			if (doneanimations == connectedPlayers) {
				animationsInProgress = false;
				doneanimations = 0;
				sendStartTurnPackets();
			}
		}
	}
}

// make this nicer
void Lobby::receiveFromClients() {
	Data packet;

	for (auto & p : players) {
		int data_length = network->receiveDataS(p->s, network_data); //network->receiveData(p->clientID, network_data);

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
			case PLAYER_ACTION:
			{
				handlePlayerAction(packet, p->clientID);
				actionsReceived++;
			}
			break;
			case ANIMATIONS_COMPLETE:
				doneanimations++;
				break;
			case INIT_CONNECTION:
				p->classType = packet.actualData;
				printf("Data:\n"); printf(packet.actualData);
				sendConnID(p);
				break;
			case ACTION_EVENT:
				handleActionEvent();
				break;
			default:
				printf("error in packet types\n");
				break;
			}
		}
	}
}

void Lobby::handleActionEvent() {
	printf("not enough players ready\n");
	// update and set that this guy is ready
	if (players.size() == 4) {
		ingame = true;
		gameStartTime = clock();
		sendStartGamePackets();
		sendStartTurnPackets();
	}

	printf("%d\n", players.size());
}

void Lobby::handlePlayerAction(Data p, int id) {
	bool aggregated = false;
	// aggreagate turn info str
	for (auto& t : turnInfo) {
		if (!aggregated) {
			// first find te right pair using the id
			if (t.first == id) {
				for (auto& s : t.second) {
					// next, find the first string that's still default
					if (s == defaultActionString) {
						// modify it
						s = p.actualData;
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
		actionsReceived = 0;
	}
}

void Lobby::sendActionPackets() {
	// send action packet
	const unsigned int packet_size = sizeof(Data);
	char packet_data[packet_size];

	Data packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);

	sendToPlayers(packet_data, packet_size);
}

void Lobby::sendConnID(Client* c) {
	// send action packet
	const unsigned int packet_size = sizeof(Data);
	char packet_data[packet_size];

	Data packet;
	packet.packet_type = INIT_CONNECTION;

	std::ostringstream oss;
	oss << c->clientID;

	packet.setData(oss.str().c_str());
	packet.serialize(packet_data);

	sendToClient(c, packet_data, packet_size);

	// network->sendToSpecific(packet_data, packet_size, id);
}

void Lobby::sendToClient(Client*& c, char* packets, int totalSize) {
	int iSendResult;
	iSendResult = NetworkHelpers::sendMessage(c->s, packets, totalSize);

	if (iSendResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(c->s);
		c->disconnected = true;
	}

	cleanupDisconnected();
}

void Lobby::sendStartGamePackets()
{
	// clear the previous turn info if any exists
	turnInfo.clear();

	char packet_data[MAX_PACKET_SIZE];

	Data packet;
	packet.packet_type = GAME_START;

	// send the ID's in order
	std::ostringstream oss;

	for (auto i : players) {
		oss << i->clientID << "," << i->classType << "|";

		// while we're at it, start to initalize the turn information for the game
		std::vector<std::string> vs;

		turnInfo.push_back(std::make_pair(i->clientID, vs));
	}

	initializeTurnInformation();

	std::string s = oss.str();
	packet.setData(s.c_str());
	packet.serialize(packet_data);

	sendToPlayers(packet_data, MAX_PACKET_SIZE);
}

void Lobby::sendStartTurnPackets()
{
	resetTurnInformation();

	char packet_data[MAX_PACKET_SIZE];

	Data packet;
	packet.packet_type = TURN_START;

	packet.serialize(packet_data);

	sendToPlayers(packet_data, MAX_PACKET_SIZE);
}

void Lobby::sendTimerPackets()
{
	char packet_data[MAX_PACKET_SIZE];

	Data packet;
	packet.packet_type = TIMER_PING;
	// add clock time
	packet.serialize(packet_data);


	sendToPlayers(packet_data, MAX_PACKET_SIZE);
	timerPacketsSent++;

	if (timerPacketsSent == 30) {
		sendTurnInformation();
		timerPacketsSent = 0;
	}
}

void Lobby::sendTurnInformation() {
	char packet_data[MAX_PACKET_SIZE];

	Data packet;
	packet.packet_type = TURN_OVER;
	// fill in turn information here

	std::ostringstream oss;

	for (int i = 0; i < ACTIONS_PER_TURN; i++) {
		for (auto& t : turnInfo) {
			/*
			if (t.second.at(i) == defaultActionString) {
			oss << t.first << "," << t.second.at(i) << "]";
			} else {
			*/
			oss << t.second.at(i) << "]";
			//}
		}
		oss << "\n";
	}

	printf("TURN INFO:\n");
	printf(oss.str().c_str());
	printf("\n");
	packet.setData(oss.str().c_str());

	packet.serialize(packet_data);

	sendToPlayers(packet_data, MAX_PACKET_SIZE);
	animationsInProgress = true;
}

void Lobby::initializeTurnInformation() {
	for (auto& t : turnInfo) {
		for (int i = 0; i < ACTIONS_PER_TURN; i++) {
			t.second.push_back(defaultActionString);
		}
	}
}

void Lobby::resetTurnInformation() {
	for (auto& t : turnInfo) {
		for (auto& s : t.second) {
			s = defaultActionString;
		}
	}
}

bool Lobby::validateTurnsComplete() {
	
	if (actionsReceived == (connectedPlayers * ACTIONS_PER_TURN)) {
		return true;
	} 

	return false;
}

void Lobby::sendToPlayers(char* packets, int totalSize) {
	int iSendResult;

	for (auto & x : players) {
		iSendResult = NetworkHelpers::sendMessage(x->s, packets, totalSize);

		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(x->s);
			x->disconnected = true;
		}
	}

	cleanupDisconnected();
}

void Lobby::cleanupDisconnected() {
	auto i = std::begin(players);

	while (i != std::end(players)) {
		// Do some stuff
		if ((*i)->disconnected) {
			i = players.erase(i);
			connectedPlayers--;
		} else {
			++i;
		}
	}

	if (connectedPlayers == 0) {
		// find a way to signify that this lobby can be removed?
	}
}