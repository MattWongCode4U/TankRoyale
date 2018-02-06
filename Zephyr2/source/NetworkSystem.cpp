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
		iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);

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
	// used to prevent the io system from posting messages too often
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
		if (echoMode) {

			if (thisTime >= timerGameTime) { // time is up
				// send stuff in turns
				broadcastTurnInfo();

				// set the new turn timer
				timerGameTime = thisTime + turnTimer;
			}
		} else {
			networkUpdate();
		}

		currentGameTime = thisTime + timeFrame;


	}
}

void NetworkSystem::handleMessage(Msg *msg) {
	// call the parent first 
	System::handleMessage(msg);

	vector<string> data = split(msg->data, ',');
	// personal call 

	switch (msg->type) {
	case NETWORK_R_IDLE:
		playerTurnAction[actionCounter] = "NETWORK_R_IDLE";
		playerTurnTarget[actionCounter] = "A0"; // can be changed to use -- later
		// always add 1 to the action counter
		actionCounter++;
		break;
	case NETWORK_R_ACTION:
		if (actionCounter > 3) {
			break;
		}
		playerTurnAction[stoi(data[2])] = data[1];
		playerTurnTargetX[stoi(data[2])] = data[3]; //seperateX and y to match gamesystem
		playerTurnTargetY[stoi(data[2])] = data[4];//seperateX and y to match gamesystem

		// always add 1 to the action counter
		//may not be needed if action # passed in from game systems
		actionCounter++;
		break;
	case NETWORK_R_PING:

		break;
	case NETWORK_S_ACTION:
		if (!echoMode) {
			aggregateTurnInfo(msg);
		}
		break;
	default:
		break;
	}
}

void NetworkSystem::aggregateTurnInfo(Msg* m) {

}

void NetworkSystem::broadcastTurnInfo() {
	std::string turnInfo = "";

	if (echoMode) {
		// needs to be changed later to use a loop in case we change max action count
		turnInfo =
			playerID + "," + playerTurnAction[0] + "," + playerTurnTargetX[0] + "," + playerTurnTargetY[0] + "]player2, MOVE, 100,100]player3, MOVE, -20,110]player4, MOVE, -120,0]\n" +
			playerID + "," + playerTurnAction[1] + "," + playerTurnTargetX[1] + "," + playerTurnTargetY[1] + "]player2, MOVE, 100,80]player3, MOVE, -20,100]player4, MOVE, -100,-10]\n" +
			playerID + "," + playerTurnAction[2] + "," + playerTurnTargetX[2] + "," + playerTurnTargetY[2] + "]player2, MOVE, 100,60]player3, MOVE, -20,90]player4, MOVE, -80,-30]\n" +
			playerID + "," + playerTurnAction[3] + "," + playerTurnTargetX[3] + "," + playerTurnTargetY[3] + "]player2, MOVE, 100,40]player3, MOVE, -20,80]player4, MOVE, -60,-50]";
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

void NetworkSystem::sendActionPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Data);
	char packet_data[packet_size];

	Data packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);

	NetworkHelpers::sendMessage(ConnectSocket, packet_data, packet_size);
}

void NetworkSystem::networkUpdate() {
	Data packet;
	int data_length = receivePackets(network_data);

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

		case ACTION_EVENT:

			OutputDebugString("client received action event packet from server\n");

			sendActionPackets();

			break;

		default:

			OutputDebugString("error in packet types\n");

			break;
		}
	}
}