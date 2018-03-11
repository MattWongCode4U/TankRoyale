#include "Client.h"

Client::Client(int id, SOCKET ss) {
	clientID = id;
	s = ss;
}