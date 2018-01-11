#include "NetworkSystem.h"


using namespace std;

NetworkSystem::NetworkSystem(MessageBus* mbus) : System(mbus) {
	m = new Msg(EMPTY_MESSAGE, "");
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
		if (thisTime  < currentGameTime) {
			Sleep(currentGameTime - thisTime);
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
			if (actionCounter > 3) { break;
			}
			playerTurnAction[stoi(data[2])] = data[1];
			playerTurnTargetX[stoi(data[2])] = data[3]; //seperateX and y to match gamesystem
			playerTurnTargetY[stoi(data[2])] = data[4];//seperateX and y to match gamesystem
			
			// always add 1 to the action counter
			//may not be needed if action # passed in from game systems
			actionCounter++;
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