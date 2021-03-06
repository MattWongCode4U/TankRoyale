#include "Server.h"

Server::Server(void) {
	// create a lobby
	lobbies.push_back(new Lobby(this));

	// create WSADATA object
	WSADATA wsaData;

	// our sockets for the server
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	// address info for the server to listen to
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}

	// set address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR) {
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// no longer need address information
	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	iResult = listen(ListenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
}

bool Server::acceptNewClient(unsigned int & id) {
	// if client waiting, accept the connection and save the socket
	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket != INVALID_SOCKET)
	{
		//disable nagle on the client's socket
		char value = 1;
		setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		// insert new client into session id table
		sessions.insert(std::pair<unsigned int, SOCKET>(id, ClientSocket));

		// make new client object and add it to the client list
		// clients.push_back(new Client(client_id, ClientSocket));

		bool inserted = false;		
		for (auto & l : lobbies) {
			if (!l->full) {
				l->players.push_back(new Client(client_id, ClientSocket));
				inserted = true;

				if (l->players.size() == 4) {
					l->full = true;
				}

				printf("Now Inserted\n");

				break;
			}
		}

		if (!inserted) {
			// make a new lobby
			// add the player to that lobby
			Lobby* l = new Lobby(this);
			l->players.push_back(new Client(client_id, ClientSocket));
			lobbies.push_back(l);
			printf("Not Inserted Now Inserted\n");
		}

		return true;
	}

	return false;
}

int Server::receiveData(unsigned int client_id, char * recvbuf)
{
	if (sessions.find(client_id) != sessions.end())
	{
		SOCKET currentSocket = sessions[client_id];
		iResult = NetworkHelpers::receiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);
		if (iResult == 0)
		{
			printf("Connection closed\n");
			closesocket(currentSocket);
		}
		return iResult;
	}
	return 0;
}

int Server::receiveDataS(SOCKET& s, char * recvbuf) {
	
	iResult = NetworkHelpers::receiveMessage(s, recvbuf, MAX_PACKET_SIZE);
	if (iResult == 0)
	{
		printf("Connection closed\n");
		closesocket(s);
	}
	return iResult;

}

void Server::sendToAll(char * packets, int totalSize)
{
	SOCKET currentSocket;
	std::map<unsigned int, SOCKET>::iterator iter;
	int iSendResult;

	for (iter = sessions.begin(); iter != sessions.end(); iter++)
	{
		currentSocket = iter->second;
		iSendResult = NetworkHelpers::sendMessage(currentSocket, packets, totalSize);

		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(currentSocket);
		}
	}
}


void Server::sendToPlaying(char* packets, int totalSize, std::vector<int> playing) {
	SOCKET currentSocket;
	std::map<unsigned int, SOCKET>::iterator iter;
	int iSendResult;

	for (int x : playing) {
		currentSocket = sessions.at(x);
		iSendResult = NetworkHelpers::sendMessage(currentSocket, packets, totalSize);

		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(currentSocket);
		}
	}
}

void Server::sendToSpecific(char* packets, int totalSize, int id) {
	SOCKET currentSocket;
	int iSendResult;
	currentSocket = sessions.at(id);
	iSendResult = NetworkHelpers::sendMessage(currentSocket, packets, totalSize);

	if (iSendResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(currentSocket);
	}

}

void Server::sendToSocket(char* packets, int totalSize, SOCKET s) {
	int iSendResult;
	
	iSendResult = NetworkHelpers::sendMessage(s, packets, totalSize);

	if (iSendResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(s);
	}
}







void Server::update() {

	// check for new connections
	if (acceptNewClient(client_id))
	{
		printf("client %d has been connected to the server\n", client_id);
		client_id++;
	}

	// update lobbies
	for (auto & l : lobbies) {
		l->update();
	}
}