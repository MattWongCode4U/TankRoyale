#pragma once
#include "Scene_Gameplay.h"
#include "GameSystem.h"

Scene_Gameplay::Scene_Gameplay(MessageBus* _mbus, GameSystem* _gs) : Scene(_mbus, _gs) {
}

Scene_Gameplay::~Scene_Gameplay() {
}

//called whene the scene is first loaded. Do any initial setup here
void Scene_Gameplay::startScene() {
	framesSinceTurnStart = 99999;
	gameSystem->addGameObjects("prototype_level.txt");
	setPlayerTank("player1");	
	gameSystem->levelLoaded = 2;
	Msg* m = new Msg(LEVEL_LOADED, "2");
	msgBus->postMessage(m, gameSystem);
	
	msgBus->postMessage(new Msg(READY_TO_START_GAME, ""), gameSystem);
}

//called every frame of the gameloop
void Scene_Gameplay::sceneUpdate() {
	//execute actions
	if (framesSinceTurnStart == 0) {
		//clear Indicators for move selection
		for (GameObject* g : gameSystem->gameObjects) {
			if (g->id.find("TileIndicator") != std::string::npos) {
				g->renderable = "nothing.png";
				msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, g->id + ",1," + g->renderable), gameSystem);
			}
		}
		executeAction(0);
		updateActionBar(4);
		turnStartTime = clock();
	}
	else if (framesSinceTurnStart == 100) {
		executeAction(1);
		updateActionBar(3);
	}
	else if (framesSinceTurnStart == 200) {
		executeAction(2);
		updateActionBar(2);
	}
	else if (framesSinceTurnStart == 300) {
		executeAction(3);
		updateActionBar(1);
		msgBus->postMessage(new Msg(NETWORK_S_ANIMATIONS, ""), gameSystem);//tells network system action animation is done on client																 //spam out actions if dead
	}
	else if (framesSinceTurnStart == 350) {
		//clear Explosions from previous actions
		for (GameObject* ex : gameSystem->gameObjects) {
			if (ex->id.find("xplosion") != std::string::npos) {
				gameSystem->gameObjects.erase(remove(gameSystem->gameObjects.begin(), gameSystem->gameObjects.end(), ex), gameSystem->gameObjects.end());
				gameSystem->gameObjectRemoved(ex);
			}
		}
		
	}
	framesSinceTurnStart++;
}

void Scene_Gameplay::updateActionBar(int a)
{
	FullscreenObj *bar = NULL;
	std::string barID = "actionBar" + to_string(a);
	for (GameObject* g : gameSystem->gameObjects) {
		if (g->id == barID)
		{
			bar = (FullscreenObj*)g;
			break;
		}
	}
	bar->renderable = "bar" + to_string(a) + "_Yellow.png";
	msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, bar->id + ",1," + bar->renderable), gameSystem);
}

//called everytime a message is received by the gameSystem
void Scene_Gameplay::sceneHandleMessage(Msg * msg) {
		std::ostringstream oss, oss2;
		Msg* mm = new Msg(EMPTY_MESSAGE, "");

		for (GameObject* g : gameSystem->gameObjects) {
			if (g->id == "reticle"&& g->getObjectType() == "GridObject") {
				gameSystem->reticle = (GridObject*)g;

				//for testing of object parenting
				/*GameObject* testObj = gameSystem->findGameObject("testObject");
				if (testObj->parentObject == nullptr) {
					testObj->setParent(g);
				}*/
				
			}
		}

		vector<string> playersArray;
		vector<string> playerAction;
		Vector2 reticleWorldPos;

		int dist;

		//messages to be read in both active and inactive game state
		switch (msg->type) {
		case NETWORK_R_START_TURN:
			gameSystem->gameActive = true;
			OutputDebugString("RECEVIED NETWORK_R_START_TURN... INPUT UNBLOCKED\n");
			actionOrigin = playerTank;

			if (playerTank->health <= 0) {
				string spoofData = gameSystem->clientID + ",3,0,0";
				msgBus->postMessage(new Msg(NETWORK_S_ACTION, spoofData), gameSystem);
				msgBus->postMessage(new Msg(NETWORK_S_ACTION, spoofData), gameSystem);
				msgBus->postMessage(new Msg(NETWORK_S_ACTION, spoofData), gameSystem);
				msgBus->postMessage(new Msg(NETWORK_S_ACTION, spoofData), gameSystem);
				gameSystem->currentAction = gameSystem->maxActions;
			}

			updateReticle();
			break;

		case NETWORK_TURN_BROADCAST:
			gameSystem->actionsToExecute = split(msg->data, '\n');
			gameSystem->currentAction = 0;
			framesSinceTurnStart = 0;
			gameSystem->gameActive = false;
			OutputDebugString("RECEVIED NETWORK_TURN_BROADCAST... INPUT BLOCKED\n");
			break;
		case NETWORK_R_GAMESTART_OK: {//"id1,ClientID,id3,id4," only for server version
			vector<string> clientIDVector = split(msg->data, ',');
			//OutputDebugString(to_string(clientIDVector.size()).c_str());

			OutputDebugString("GS: NETWORK_R_GAMESTART_OK RECEIVED:  ");
			OutputDebugString(msg->data.c_str());
			OutputDebugString("\n");

			for (int i = 0; i < clientIDVector.size(); i++) {
				if (clientIDVector[i] == gameSystem->clientID) {
					//set the new player tank
					setPlayerTank("player" + to_string(i + 1));
				}
			}
			break;
		}
		case NETWORK_R_PING:
			gameSystem->displayTimeLeft(stoi(msg->data));
			break;
		default:
			break;
		}
		//messages that are only read when game is in active state (used for blocking player input during animations)

		if (gameSystem->gameActive) switch (msg->type) {
		case MOUSE_MOVE:
		{
			vector<string> objectData = split(msg->data, ',');
			INT32 x = atoi(objectData[0].c_str());
			INT32 y = atoi(objectData[1].c_str());
			INT32 width = atoi(objectData[2].c_str());
			INT32 length = atoi(objectData[3].c_str());
			x -= width / 2; y -= length / 2;
			y = -y;

			int offsetX = x;
			int offsetY = y;

			if (y < -44) offsetY -= 45;
			else if (y > 44) offsetY += 45;

			int gridLocationY = (offsetY * 2 / 3) / gameSystem->hexSize;

			if (x < -44 && gridLocationY % 2 == 0) offsetX -= 45;
			else if (x > 44 && gridLocationY % 2 == 0) offsetX += 45;
			else if (x <= 0 && gridLocationY % 2 != 0) offsetX -= 90;

			int gridLocationX = (offsetX * sqrt(3) / 3) / gameSystem->hexSize;

			gameSystem->reticle->gridX = gridLocationX;
			gameSystem->reticle->gridY = gridLocationY;
			updateReticle();

			break;
		}
		case DOWN_ARROW_PRESSED: {
			gameSystem->reticle->gridY--;
			updateReticle();
			break;
		}
		case UP_ARROW_PRESSED:
			gameSystem->reticle->gridY++;
			updateReticle();
			break;

		case LEFT_ARROW_PRESSED:
			gameSystem->reticle->gridX--;
			updateReticle();
			break;

		case RIGHT_ARROW_PRESSED:
			gameSystem->reticle->gridX++;
			updateReticle();
			break;

		case SPACEBAR_PRESSED: {
			if (gameSystem->currentAction >= gameSystem->maxActions || !validMove) break;
			if (ActionType == SHOOT && gameSystem->currentAction >= gameSystem->maxActions - 1) break;

			//Send Message to network
			//message format: playerID,actionName,actionNumber,targetX,targetY
			oss << gameSystem->clientID << ","//playerID
				<< to_string(ActionType) << ","//action type (e.g. MOVE or SHOOT)
											   //<< currentAction << ","//the action number 0 to <number of actions/turn>
				<< gameSystem->reticle->gridX << "," //target x pos
				<< gameSystem->reticle->gridY; //target y pos

			mm->type = NETWORK_S_ACTION;
			mm->data = oss.str();
			msgBus->postMessage(mm, gameSystem);

			if (ActionType == SHOOT)	// DUMMY PASSING DATA AFTER SHOOT
			{
				oss2 << gameSystem->clientID << ","//playerID
					<< to_string(PASS) << ","//action type (e.g. MOVE or SHOOT)
												   //<< currentAction << ","//the action number 0 to <number of actions/turn>
					<< gameSystem->reticle->gridX << "," //target x pos
					<< gameSystem->reticle->gridY; //target y pos

				msgBus->postMessage(new Msg(NETWORK_S_ACTION, oss2.str()), gameSystem);
			}

			GridObject *indicator = NULL;
			FullscreenObj *bar = NULL, *bar2 = NULL;

			string indicatorId = "TileIndicator" + to_string(gameSystem->currentAction);
			string actionBarId = "actionBar" + to_string(gameSystem->currentAction + 1);
			string actionBarId2;
			if (gameSystem->currentAction <= gameSystem->maxActions - 1 && ActionType == SHOOT)
			{
				actionBarId2 = "actionBar" + to_string(gameSystem->currentAction + 2);
			}
			

			for (GameObject* g : gameSystem->gameObjects) {
				if (g->id == indicatorId) {
					indicator = (GridObject*)g;
				}
				if (g->id == actionBarId)
				{
					bar = (FullscreenObj*)g;
				}
				if (!actionBarId2.empty() && g->id == actionBarId2)
				{
					bar2 = (FullscreenObj*)g;
				}
			}

			bar->renderable = "bar" + to_string(gameSystem->currentAction + 1) + "_Green.png";
			if (ActionType == MOVE) 
			{
				actionOrigin = indicator;
				indicator->renderable = "MoveIndicator.png";
			}
			else if (ActionType == SHOOT)
			{
				indicator->renderable = "ShootIndicator.png";
				if (bar2 != NULL)
				{
					bar2->renderable = "bar" + to_string(gameSystem->currentAction + 2) + "_Green.png";
				}
			}

			indicator->gridX = gameSystem->reticle->gridX;
			indicator->gridY = gameSystem->reticle->gridY;
			indicator->updateWorldCoords();
			//indicator->renderable = "TileIndicatorNum" + to_string(currentAction) + ".png";
			gameSystem->sendUpdatePosMessage(indicator);//send indicator position message

			//send update sprite message. maybe this sould be included in update position?
			msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, indicator->id + ",1," + indicator->renderable), gameSystem);
			msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, bar->id + ",1," + bar->renderable), gameSystem);
			if (bar2 != NULL)
			{
				msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, bar2->id + ",1," + bar2->renderable), gameSystem);
			}

			if (ActionType == MOVE) 
			{
				gameSystem->currentAction++;
			}
			else if (ActionType == SHOOT)
			{
				gameSystem->currentAction += 2;
			}
			

			break;
		}
		case KEY_A_PRESSED:
			(ActionType == MOVE) ? setActionType(SHOOT) : setActionType(MOVE);
			updateReticle();
			break;

		case KEY_D_PRESSED: {
			gameSystem->findGameObject("testObject")->offsetPosition(0, 0, 0, 30);

			//return the position of the reticle for debugging purposes
			string s = "RETICLE AT GRID("
				+ to_string(gameSystem->reticle->gridX)
				+ "," + to_string(gameSystem->reticle->gridY)
				+ ")  WORLD("
				+ to_string(gameSystem->reticle->x)
				+ "," + to_string(gameSystem->reticle->y)
				+ ")\n";
			OutputDebugString(s.c_str());
			break;
		}
		case TEST_KEY_PRESSED:
			break;

		case SHOOT_CANNON:
			break;

		case GO_COLLISION:
			break;

		case UPDATE_OBJECT_POSITION: {

			vector<string> data = split(msg->data, ',');

			for (GameObject* g : gameSystem->gameObjects) {

				if (g->id == data[0]) {
					g->x = stof(data[2].c_str());
					g->y = stof(data[3].c_str());
					g->orientation = stoi(data[5].c_str());
				}
			}
			break;
		}
		case KEY_Z_PRESSED:
			// PREVIOUSLY USED FOR TESTING HEALTHBAR // NOW UNUSED
			break;
		default:
			break;
		}
	}

void Scene_Gameplay::executeAction(int a) {
	//clear Explosions from previous actions
	for (GameObject* ex : gameSystem->gameObjects) {
		if (ex->id.find("xplosion") != std::string::npos) {
			gameSystem->gameObjects.erase(remove(gameSystem->gameObjects.begin(), gameSystem->gameObjects.end(), ex), gameSystem->gameObjects.end());
			gameSystem->gameObjectRemoved(ex);
		}
	}

	vector<string> playerAction;
	vector<string> players = split(gameSystem->actionsToExecute[a], ']');

	for (int playerNum = 0; playerNum < 4; playerNum++) {

		playerAction = split(players[playerNum], ',');
		string currentObjectId = "player" + to_string(playerNum + 1);//get id from the order of incoming actions
		ActionTypes receivedAction = static_cast<ActionTypes>(stoi(playerAction[1]));//parse action type

		//switch on the action type received from the network system, and execute the action
		switch (receivedAction) {
		case SHOOT: {
			string newID = "explosion" + to_string(rand());
			GridObject* gr = new GridObject(&(gameSystem->objData), newID, "explosion.png", 0, 0, 4, 0, 250, 250, 1, stoi(playerAction[2]), stoi(playerAction[3]), "");
				
			gameSystem->createGameObject(gr);
			gr->updateWorldCoords();
			dealAOEDamage(stoi(playerAction[2]), stoi(playerAction[3]), 1, 19);
			break;
		}

		case MOVE:
			//display player MOVE actions for players whose id's are found
			for (GameObject* g : gameSystem->gameObjects) {
				if (g->id == currentObjectId) {
					//OutputDebugString(playerAction[2].c_str());
					TankObject* t = (TankObject*)g;
					if (t->health > 0) {
						t->gridX = stoi(playerAction[2]);
						t->gridY = stoi(playerAction[3]);
						t->updateWorldCoords();
						gameSystem->sendUpdatePosMessage(t);
					}
				}
			}
			break;
		case PASS:
		{
			std::cout << "Turn passed";
			break;
		}
		}
		
	}
	
}

void Scene_Gameplay::setActionType(ActionTypes a) {
	ActionType = a;
	std::ostringstream oss;
	Msg* mm;

	switch (a) {
	case SHOOT:
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "shootIcon,1,Reticle.png"), gameSystem);
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "moveIcon,1,moveIconInactive.png"), gameSystem);
		range = 5;
		break;
	case MOVE:
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "shootIcon,1,ReticleInactive.png"), gameSystem);
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "moveIcon,1,moveIconActive.png"), gameSystem);
		range = 1;
		break;
	}
}

void Scene_Gameplay::dealAOEDamage(int _originX, int _originY, int affectedRadius, int damage) {
	int aXCube = _originX - (_originY - (_originY & 1)) / 2;
	int aZCube = _originY;
	int aYCube = -aXCube - aZCube;
	OutputDebugString("origin point of shot: ");
	OutputDebugString(to_string(_originX).c_str());
	OutputDebugString(" , ");
	OutputDebugString(to_string(_originY).c_str());
	OutputDebugString("\n");

	for (GameObject *go : gameSystem->gameObjects) { //look through all gameobjects
		if (go->getObjectType() == "TankObject") {
			TankObject* tank = (TankObject*)go;
			if (getGridDistance(_originX, _originY, tank->gridX, tank->gridY) <= affectedRadius) {
				tank->health -= damage;
				OutputDebugString(tank->id.c_str());
				OutputDebugString(" GOT HIT AT:");
				OutputDebugString(to_string(tank->gridX).c_str());
				OutputDebugString(" , ");
				OutputDebugString(to_string(tank->gridY).c_str());
				OutputDebugString("\n");

				if (tank->health <= 0)
					msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, tank->id + ",1,crater.png,"), gameSystem);
				updatePlayerHealthBar(tank->id);
			}
		}
	}
}

int Scene_Gameplay::getGridDistance(int aX, int aY, int bX, int bY) {
	int aXCube = aX - (aY - (aY & 1)) / 2;
	int aZCube = aY;
	int aYCube = -aXCube - aZCube;

	int bXCube = bX - (bY - (bY & 1)) / 2;
	int bZCube = bY;
	int bYCube = -bXCube - bZCube;

	return (abs(aXCube - bXCube) + abs(aYCube - bYCube) + abs(aZCube - bZCube)) / 2;
}

void Scene_Gameplay::updateReticle() {
	gameSystem->reticle->updateWorldCoords();
	//gameSystem->sendUpdatePosMessage(gameSystem->reticle);

	int dist = getGridDistance(gameSystem->reticle->gridX, gameSystem->reticle->gridY, actionOrigin->gridX, actionOrigin->gridY);

	if (dist > range) {
		gameSystem->reticle->renderable = "TileIndicatorRed.png";
		validMove = false;
	}
	else {
		gameSystem->reticle->renderable = "TileIndicator.png";
		validMove = true;
	}

	std::ostringstream oss;
	Msg* mm = new Msg(EMPTY_MESSAGE, "");

	//UPDATE_OBJECT_SPRITE, //id,#Frames,Renderable
	msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, gameSystem->reticle->id + ",1," + gameSystem->reticle->renderable), gameSystem);
}

//	Takes in the player number we are going to be udpating. Enum in GameSystem
void Scene_Gameplay::updatePlayerHealthBar(string playerID) {
	Msg* m;
	TankObject* curPlayer = nullptr;
	FullscreenObj* curHealthBar = nullptr;
	curPlayer = gameSystem->findTankObject(playerID);
	curHealthBar = gameSystem->findFullscreenObject(playerID + "_hpbar");
	if (curPlayer != nullptr && curHealthBar != nullptr) {
		int hpBarSize = (int)((float)(curHealthBar->originalWidth * ((float)curPlayer->getHealth() / (float)TANK_MAX_HEALTH))); // TEST: Does this update the size correctly?
		if (curPlayer->getHealth() == 100) {
			std::ostringstream oss;
			//id,renderable,x,y,z,orientation,width,length
			oss << curHealthBar->id << ",";
			oss << curHealthBar->renderable << ",";
			oss << curHealthBar->x << ",";
			oss << curHealthBar->y << ",";
			oss << curHealthBar->z << ",";
			oss << curHealthBar->orientation << ",";
			oss << curHealthBar->originalWidth << ",";
			oss << curHealthBar->length;
			m = new Msg(MSG_TYPE::UPDATE_HP_BAR, oss.str());
		}
		else if (curPlayer->getHealth() <= 30) {
			std::ostringstream oss;
			// change sprite
			/*if (curHealthBar->renderable != "red_hpbar.png")
			{
			oss << curHealthBar->id << ",";
			oss << " ,";
			oss << "red_hpbar.png";
			m = new Msg(MSG_TYPE::UPDATE_OBJ_SPRITE, oss.str());
			msgBus->postMessage(m, this);
			}*/
			// update size
			oss.clear();
			oss << curHealthBar->id << ",";
			oss << curHealthBar->renderable << ",";
			oss << curHealthBar->x << ",";
			oss << curHealthBar->y << ",";
			oss << curHealthBar->z << ",";
			oss << curHealthBar->orientation << ",";
			oss << hpBarSize << ","; // width
			oss << curHealthBar->length; // lenght
			m = new Msg(MSG_TYPE::UPDATE_OBJECT_POSITION, oss.str());
		}
		else if (curPlayer->getHealth() <= 50) {
			std::ostringstream oss;
			/*if (curHealthBar->renderable != "orange_hpbar.png")
			{
			oss << curHealthBar->id << ",";
			oss << curHealthBar->id << ",";
			oss << "orange_hpbar.png";
			m = new Msg(MSG_TYPE::UPDATE_OBJ_SPRITE, oss.str());
			msgBus->postMessage(m, this);
			}*/
			oss.clear();
			oss << curHealthBar->id << ",";
			oss << curHealthBar->renderable << ",";
			oss << curHealthBar->x << ",";
			oss << curHealthBar->y << ",";
			oss << curHealthBar->z << ",";
			oss << curHealthBar->orientation << ",";
			oss << hpBarSize << ","; // width
			oss << curHealthBar->length; // lenght
			m = new Msg(MSG_TYPE::UPDATE_OBJECT_POSITION, oss.str());
		}
		else {
			std::ostringstream oss;
			/*if (curHealthBar->renderable != "green_hpbar.png")
			{
			oss << curHealthBar->id << ",";
			oss << " ,";
			oss << "green_hpbar.png";
			m = new Msg(MSG_TYPE::UPDATE_OBJ_SPRITE, oss.str());
			msgBus->postMessage(m, this);
			}*/
			oss.clear();
			oss << curHealthBar->id << ",";
			oss << curHealthBar->renderable << ",";
			oss << curHealthBar->x << ",";
			oss << curHealthBar->y << ",";
			oss << curHealthBar->z << ",";
			oss << curHealthBar->orientation << ",";
			oss << hpBarSize << ","; // width
			oss << curHealthBar->length; // lenght
			m = new Msg(MSG_TYPE::UPDATE_OBJECT_POSITION, oss.str());
		}
		msgBus->postMessage(m, gameSystem);
	}
};

void Scene_Gameplay::setPlayerTank(std::string playerID) {
	if (playerTank != nullptr) {
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, playerTank->id + ",1,sciFiTank2.png,"), gameSystem);
	}
	for (GameObject* g : gameSystem->gameObjects) {
		if (g->id == playerID && g->getObjectType() == "TankObject") {
			playerTank = (TankObject*)g;
			actionOrigin = playerTank;
			msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, playerTank->id + ",1,sciFiTank.png,"), gameSystem);

			string debugS = "PLAYER POINTER SET TO: " + playerID + "\n";
			OutputDebugString(debugS.c_str());
		}
	}
}