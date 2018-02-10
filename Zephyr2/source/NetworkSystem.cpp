#include "NetworkSystem.h"

using namespace std;

NetworkSystem::NetworkSystem(MessageBus* mbus) : System(mbus) {
	m = new Msg(EMPTY_MESSAGE, "");

	if (!echoMode) {
		// create WSADATA object
		WSADATA wsaData;

		// socket
		ConnectSocket = INVALID_SOCKET;

		// holds address info for socket to connect to
		struct addrinfo *result = NULL,
			*ptr = NULL,
			hints;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			exit(1);
		}

		// set address info
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		//resolve server address and port
		iResult = getaddrinfo("50.64.164.103", DEFAULT_PORT, &hints, &result);

		if (iResult != 0)
		{
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			exit(1);
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);

			if (ConnectSocket == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
				WSACleanup();
				exit(1);
			}

			// Connect to server.
			iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

			if (iResult == SOCKET_ERROR)
			{
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				printf("The server is down... did not connect");
			}
		}
		// no longer need address info for server
		freeaddrinfo(result);

		// if connection failed
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("Unable to connect to server!\n");
			WSACleanup();
			exit(1);
		}

		u_long iMode = 1;

		iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
		if (iResult == SOCKET_ERROR)
		{
			printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			exit(1);
		}

		// send init packet
		const unsigned int packet_size = sizeof(Data);
		char packet_data[packet_size];

		Data packet;
		packet.packet_type = INIT_CONNECTION;

		packet.serialize(packet_data);

		NetworkHelpers::sendMessage(ConnectSocket, packet_data, packet_size);
	}
}

NetworkSystem::~NetworkSystem()
{
}

void NetworkSystem::startSystemLoop() {
	clock_t thisTime = clock();

	int currentGameTime = 0;
	int timerGameTime = 0;

	while (alive) {
		thisTime = clock();
		if (thisTime < currentGameTime) {
			std::this_thread::sleep_for(std::chrono::milliseconds(currentGameTime - thisTime));
		}
		handleMsgQ();

		// if testing mode
		if (echoMode && timerActive) {
			int timeLeft = 30 - ((clock() - turnStartTime) / 1000);

			//post the time left message (includes the actual time left in the message data)
			m->type = NETWORK_R_PING;
			m->data = to_string(timeLeft);
			msgBus->postMessage(m, this);

			if (timeLeft <= 0) {
				broadcastTurnInfo();
				timerActive = false;
			}

		} 
		else {
			networkUpdate();
		}

		currentGameTime = thisTime + timeFrame;


	}
}

// this only needs to handle NETWORK_S_messages; for when user input is decided 
// it will send out NETWORK_R message when it receives from server
void NetworkSystem::handleMessage(Msg *msg) {
	// call the parent first 
	System::handleMessage(msg);

	vector<string> data = split(msg->data, ',');
	switch (msg->type) {
		case NETWORK_S_ACTION:
			aggregateTurnInfo(msg);
			break;
		case READY_TO_START_GAME:
			if (!echoMode) {
				sendActionPackets();
			}
			else {
				startTimer();
				msgBus->postMessage(new Msg(NETWORK_R_START_TURN,""), this);
				OutputDebugString("SENT NETWORK_R_START_TURN from NetworkSystem\n");

				//testing sending NETWORK_R_GAMESTART_OK message
				msgBus->postMessage(new Msg(NETWORK_R_GAMESTART_OK, "id2,defaultClientID,id3,id4,"), this);
			}
			break;
		case NETWORK_S_ANIMATIONS:
			if (!echoMode) {
				sendPacket(ANIMATIONS_COMPLETE, "");
			}
			else {
				startTimer();
				msgBus->postMessage(new Msg(NETWORK_R_START_TURN, ""), this);
				OutputDebugString("SENT NETWORK_R_START_TURN from NetworkSystem\n");
			}
			break;
		default:
			break;
	}
}

void NetworkSystem::aggregateTurnInfo(Msg* m) {
	
	if (actionCounter > 3) {
		return;
	}
	if (echoMode) {
		vector<string> data = split(m->data, ',');

		playerTurnAction[stoi(data[2])] = data[1];
		playerTurnTargetX[stoi(data[2])] = data[3]; //seperateX and y to match gamesystem
		playerTurnTargetY[stoi(data[2])] = data[4];//seperateX and y to match gamesystem

												   // always add 1 to the action counter
												   //may not be needed if action # passed in from game systems
		actionCounter++;
	} else {
		// send turn info to server
		sendPacket(PLAYER_ACTION, m->data);
	}
}

// this is only called in echomode
void NetworkSystem::broadcastTurnInfo() {
	std::string turnInfo = "";

	if (echoMode) {
		// needs to be changed later to use a loop in case we change max action count
		turnInfo =
			"player2, 0, 1,3]" + playerID + "," + playerTurnAction[0] + "," + playerTurnTargetX[0] + "," + playerTurnTargetY[0] + "]player3, 0, -2,1]player4, 0, -1,0]\n" +
			"player2, 0, 1,3]" + playerID + "," + playerTurnAction[1] + "," + playerTurnTargetX[1] + "," + playerTurnTargetY[1] + "]player3, 0, -2,3]player4, 0, -1,-1]\n" +
			"player2, 0, 1,3]" + playerID + "," + playerTurnAction[2] + "," + playerTurnTargetX[2] + "," + playerTurnTargetY[2] + "]player3, 0, -1,0]player4, 0, -3,-3]\n" +
			"player2, 0, 1,3]" + playerID + "," + playerTurnAction[3] + "," + playerTurnTargetX[3] + "," + playerTurnTargetY[3] + "]player3, 0, -2,4]player4, 0, -2,-4]";
	}

	// im being lazy here and just sending out the string since ideally the network class doesn't know how to parse. alternatively i can parse here, depends on
	// what we decide later
	Msg* tm = new Msg(NETWORK_TURN_BROADCAST, turnInfo);
	msgBus->postMessage(tm, this);

	// reset the action counter when we broadcast 
	actionCounter = 0;
}

int NetworkSystem::receivePackets(char * recvbuf) {
	iResult = NetworkHelpers::receiveMessage(ConnectSocket, recvbuf, MAX_PACKET_SIZE);

	if (iResult == 0)
	{
		printf("Connection closed\n");
		closesocket(ConnectSocket);
		WSACleanup();
		exit(1);
	}

	return iResult;
}

void NetworkSystem::sendPacket(DataType d, std::string data) {
	char packet_data[MAX_PACKET_SIZE];

	Data packet;
	packet.packet_type = d;
	packet.setData(data.c_str());
	packet.serialize(packet_data);

	if (NetworkHelpers::sendMessage(ConnectSocket, packet_data, MAX_PACKET_SIZE) == -1) {
		// send failed
		OutputDebugString("SEND FAILED");
	}
}

void NetworkSystem::sendActionPackets()
{
	// send action packet
	char packet_data[MAX_PACKET_SIZE];

	Data packet;
	packet.packet_type = ACTION_EVENT;
	/*std::ostringstream oss;
	oss << "test values[ " << rand() << " ]";
	std::string st = oss.str();
	packet.setData(st.c_str());*/
	packet.serialize(packet_data);

	/*OutputDebugString("SENDING: ");
	OutputDebugString(packet.actualData);
	OutputDebugString("\n");*/

	if (NetworkHelpers::sendMessage(ConnectSocket, packet_data, MAX_PACKET_SIZE) == -1) {
		// send failed
		OutputDebugString("SEND FAILED");
	}
}

void NetworkSystem::networkUpdate() {
	Data packet;
	int data_length = receivePackets(network_data);
	Msg* m = new Msg(EMPTY_MESSAGE);

	if (data_length <= 0)
	{
		//no data recieved
		return;
	}

	int i = 0;
	while (i < (unsigned int)data_length)
	{
		packet.deserialize(&(network_data[i]));
		i += sizeof(Data);

		switch (packet.packet_type) {
		case INIT_CONNECTION:
			OutputDebugString("NS:INIT CONNECTION\n");
			// the data in this is your playerID
			OutputDebugString(packet.actualData);
			OutputDebugString("\n");
			m->type = NETWORK_CONNECT;
			m->data = packet.actualData;
			msgBus->postMessage(m, this);
			break;
		case GAME_START:
			OutputDebugString("NS:GAME START\n");
			OutputDebugString(packet.actualData);
			OutputDebugString("\n");
			// broadcast game start
			m->type = NETWORK_R_GAMESTART_OK;
			msgBus->postMessage(m, this);
			break;
		case TIMER_PING:
			// OutputDebugString("NS:TIMER PING\n");
			timerValue--;
			m->type = NETWORK_R_PING;
			m->data = to_string(timerValue);
			msgBus->postMessage(m, this);
			break;
		case TURN_START:
			OutputDebugString("NS:TURN START\n");
			timerValue = 30;
			m->type = NETWORK_R_START_TURN;
			msgBus->postMessage(m, this);
			actionCounter = 0; // do this here instead of during Turn_over in case theres a bug where u can make actions during animation sequence
			break;
		case TURN_OVER:
			OutputDebugString("NS:TURN OVER\n");
			OutputDebugString(packet.actualData);
			OutputDebugString("\n");
			m->type = NETWORK_TURN_BROADCAST;
			m->data = packet.actualData;
			msgBus->postMessage(m, this);
			break;
		default:
			OutputDebugString("error in packet types\n");
			break;
		}
	}
}

void NetworkSystem::startTimer() {
	turnStartTime = clock();
	timerActive = true;
	
}