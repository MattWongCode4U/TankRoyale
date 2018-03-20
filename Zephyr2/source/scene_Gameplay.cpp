#pragma once
#include "Scene_Gameplay.h"
#include "GameSystem.h"

Scene_Gameplay::Scene_Gameplay(MessageBus* _mbus, GameSystem* _gs) : Scene(_mbus, _gs) {
}

Scene_Gameplay::~Scene_Gameplay() {
}

//called whene the scene is first loaded. Do any initial setup here
void Scene_Gameplay::startScene() {
	gameSystem->currentAction = 0;
	framesSinceTurnStart = 99999;
	gameSystem->addGameObjects("prototype_level.txt");
	setPlayerTank("player1");	
	gameSystem->levelLoaded = 2;
	Msg* m = new Msg(LEVEL_LOADED, "2");
	msgBus->postMessage(m, gameSystem);
	
	validActionsOverlay = gameSystem->findGridObject("validActionsOverlay");
	
	//gameSystem->reticle = gameSystem->findGridObject("reticle");
	if (gameSystem->reticle = gameSystem->findGridObject("reticle")) {
		GameObject* g = gameSystem->makeGameObject("reticleActionIndicator.txt");
		g->parentId = gameSystem->reticle->id;
		gameSystem->createGameObject(g);
		actionIndicator = g;
		}
	setActionType(ROTATEPOS);
		
	msgBus->postMessage(new Msg(READY_TO_START_GAME, gameSystem->tankClass), gameSystem);
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
	else if (framesSinceTurnStart == 400) {//actions animation done, return control to player
		//setActionType(ActionType);
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

		/*if (!gameSystem->reticle) {
			gameSystem->reticle = gameSystem->findGridObject("reticle");
		}*/
			

		vector<string> playersArray;
		vector<string> playerAction;
		Vector2 reticleWorldPos;

		int dist;

		//messages to be read in both active and inactive game state
		switch (msg->type) {
		case NETWORK_R_START_TURN:
			gameActive = true;
			OutputDebugString("RECEVIED NETWORK_R_START_TURN... INPUT UNBLOCKED\n");
			updateActionOrigin(playerTank);

			if (playerTank != nullptr && playerTank->health <= 0) {
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
			gameActive = false;

			validActionsOverlay->renderable = "nothing.png";
			validActionsOverlay->postSpriteMsg();


			OutputDebugString("RECEVIED NETWORK_TURN_BROADCAST... INPUT BLOCKED\n");

			break;
		case NETWORK_R_GAMESTART_OK: {//id1,class1|id2,class2|etc.
			vector<string> clientIDVector = split(msg->data, '|');
			vector<string> tankIdClassVector;
			//OutputDebugString(to_string(clientIDVector.size()).c_str());

			OutputDebugString("GS: NETWORK_R_GAMESTART_OK RECEIVED:  ");
			OutputDebugString(msg->data.c_str());
			OutputDebugString("\n");

			//Set tank class info here
			for (int i = 0; i < clientIDVector.size(); i++) {
				tankIdClassVector = split(clientIDVector[i], ',');

				

				//create Tank Object
				TankObject* t = (TankObject*)gameSystem->makeGameObject("Tank_" + tankIdClassVector[1] + ".txt");
				t->id = "player" + to_string(i+1);

				//set starting pos
				if (i == 0) {
					t->gridX = -3;
					t->gridY = 3;
				} else if (i == 1) {
					t->gridX = -3;
					t->gridY = -3;
				}
				else if (i == 2) {
					t->gridX = 3;
					t->gridY = 3;
				}
				else if (i == 3) {
					t->gridX = 3;
					t->gridY = -3;
				}
			
				gameSystem->createGameObject(t);
				t->updateWorldCoords();
				t->createhpBar();//create the hp bar
				

				if (tankIdClassVector[0] == gameSystem->clientID) {
					//set the new player tank
					//setPlayerTank("player" + to_string(i + 1));
					playerTank = t;
					//updateActionOrigin(playerTank);
					

					//initialize the player's orientation
					queuedOrientation = playerTank->zRotation;

					updateActionOrigin(playerTank);

					//create the playerTank Indicator
					GameObject* arrow = gameSystem->makeGameObject("arrow.txt");
					arrow->id = "playerArrow";
					arrow->parentId = t->id;
					gameSystem->createGameObject(arrow);//add the arrow to gameobjects

					OutputDebugString("\nActionORigin id : ");
					OutputDebugString(actionOrigin->id.c_str());
				}

				//create healthbar for each player
				//gameSystem->findTankObject("player" + to_string(i + 1))->createhpBar();
			}


			break;
		}
		case NETWORK_R_PING:
			gameSystem->displayTimeLeft(stoi(msg->data));
			if(stoi(msg->data) == 1 )
				for (int i = gameSystem->maxActions - gameSystem->currentAction; i > 0; i--) {
					sendNetworkActionMsg(PASS);
				}
				
			break;
		default:
			break;
		}
		//messages that are only read when game is in active state (used for blocking player input during animations)

		if (gameActive) switch (msg->type) {
		case MOUSE_MOVE:
		{

			vector<string> objectData = split(msg->data, ',');
			INT32 x = atoi(objectData[0].c_str());
			INT32 y = atoi(objectData[1].c_str());
			INT32 width = atoi(objectData[2].c_str());
			INT32 length = atoi(objectData[3].c_str());
			y = length - y;

			//SDL_Log("X %d, Y %d", x, y);
			glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)width / (float)length, 1.0f, 1000.0f);
			glm::mat4 look = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

			glm::mat4 translation = glm::translate(look, position * -1.0f);

			rotation2 = glm::mat4();

			rotation2 = glm::rotate(rotation2, rotation.z, glm::vec3(0, 0, 1));
			rotation2 = glm::rotate(rotation2, rotation.x, glm::vec3(1, 0, 0));
			rotation2 = glm::rotate(rotation2, rotation.y, glm::vec3(0, 1, 0));

			glm::mat4 view = rotation2 * translation;
			
			glm::vec3 rayOrigin = glm::unProject(glm::vec3(x, y, 0.0f), view, projection, glm::vec4(0, 0, width, length));
			glm::vec3 rayEnd = glm::unProject(glm::vec3(x, y, 1.0f), view, projection, glm::vec4(0, 0, width, length));
			glm::vec3 ray = rayEnd - rayOrigin;

			glm::vec3 normal = glm::vec3(0, 0, 1);
			float d = glm::dot(normal, glm::vec3(0, 0, -80));

			float t = (d - glm::dot(normal, rayOrigin)) / glm::dot(normal, ray);
			glm::vec3 contact = rayOrigin + ray * t;

			float distance = glm::length(contact - position);

			float offsetX = contact.x;
			float offsetY = contact.y;

			if (offsetY < -5) offsetY -= 5;
			else if (offsetY > 5) offsetY += 5;

			int gridLocationY = (offsetY * 2 / 3.0f) / gameSystem->hexSize;

			if (offsetX < -5 && gridLocationY % 2 == 0) offsetX -= 5;
			else if (offsetX > 5 && gridLocationY % 2 == 0) offsetX += 5;
			else if (offsetX <= 0 && gridLocationY % 2 != 0) offsetX -= 10;

			int gridLocationX = (offsetX * sqrt(3) / 3.0f) / gameSystem->hexSize;

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

			OutputDebugString("\n");
			OutputDebugString(to_string(queuedOrientation).c_str());

			//queuedOrientation = (queuedOrientation + 360) % 360;
			//actionOrigin->zRotation = queuedOrientation;
			updateActionOrigin(actionOrigin);

			if (gameSystem->currentAction >= gameSystem->maxActions || moveCost <= 0) break;
			if (gameSystem->currentAction > gameSystem->maxActions - moveCost) break;

			

			//send the action to the network system
			sendNetworkActionMsg(ActionType);

			//update the quedRotation
			if (ActionType == ROTATENEG)
				queuedOrientation += 60;
			else if (ActionType == ROTATEPOS)
				queuedOrientation -= 60;

			/*queuedOrientation = (queuedOrientation + 360) % 360;
			actionOrigin->zRotation = queuedOrientation;*/
			updateActionOrigin(actionOrigin);

			//pad with empty actions iff the move cost is > 1;
			for (int i = 1; i < moveCost; i++) {
				sendNetworkActionMsg(PASS);
			}

			GridObject *indicator = NULL;
			FullscreenObj *bar = NULL, *bar2 = NULL;

			string indicatorId = "TileIndicator" + to_string(gameSystem->currentAction);
			string actionBarId = "actionBar" + to_string(gameSystem->currentAction + 1);
			string actionBarId2;
			if (gameSystem->currentAction <= gameSystem->maxActions - 1 && moveCost > 1)
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
				updateActionOrigin(indicator);

				/*queuedOrientation = (queuedOrientation + 360) % 360;
				actionOrigin->zRotation = queuedOrientation;*/
				updateActionOrigin(actionOrigin);

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
			gameSystem->sendUpdatePosMessage(indicator);//send indicator position message

			//send update sprite message. maybe this sould be included in update position?
			msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, indicator->id + ",1," + indicator->renderable), gameSystem);
			msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, bar->id + ",1," + bar->renderable), gameSystem);
			if (bar2 != NULL)
			{
				msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, bar2->id + ",1," + bar2->renderable), gameSystem);
			}

			updateActionOrigin(actionOrigin);
			gameSystem->currentAction += moveCost;
			break;
		}
		case KEY_A_PRESSED:
			(ActionType == MOVE) ? setActionType(SHOOT) : setActionType(MOVE);
			
			break;

		case KEY_D_PRESSED: {

			break;
		}
		case KEY_W_PRESSED: {
			setActionType(MOVE);
			
			break;
		}
		case KEY_S_PRESSED: {
			setActionType(SHOOT);
			
			break;
		}
		case TEST_KEY_PRESSED:
			break;

		case SHOOT_CANNON:
			break;

		case GO_COLLISION:
			break;

		case UPDATE_OBJECT_POSITION: {
			//TODO: Needs to updated for new renderer or removed
			/*vector<string> data = split(msg->data, ',');

			for (GameObject* g : gameSystem->gameObjects) {

				if (g->id == data[0]) {
					g->x = stof(data[2].c_str());
					g->y = stof(data[3].c_str());
					g->orientation = stoi(data[5].c_str());
				}
			}*/
			break;
		}
		case KEY_Z_PRESSED:
			// PREVIOUSLY USED FOR TESTING HEALTHBAR // NOW UNUSED
			OutputDebugString("Current location: ");
			OutputDebugString(to_string(gameSystem->reticle->gridX).c_str());
			OutputDebugString(", ");
			OutputDebugString(to_string(gameSystem->reticle->gridY).c_str());
			OutputDebugString("\n");
			break;
		//TESTING ROTATION
		case KEY_Q_PRESSED:
			setActionType(ROTATENEG);
			break;
		case KEY_E_PRESSED:
			setActionType(ROTATEPOS);
			break;
		
		case KEY_ESC_RELEASED:
			// Pause the game (lock control, display 2 buttons)
			loadPauseMenu();
			gameActive = false;
			break;
		default:
			break;
		}
		else {
			// can still move mouse around on game scene
			vector<string> objectData;
			INT32 x, y, width, length;
			bool change;
			switch (msg->type) {
			case LEFT_MOUSE_BUTTON:
				objectData = split(msg->data, ',');
				x = atoi(objectData[0].c_str());
				y = atoi(objectData[1].c_str());
				width = atoi(objectData[2].c_str());
				length = atoi(objectData[3].c_str());
				x -= width / 2; y -= length / 2;
				y = -y;
				
				// press one of the buttons
				// 0 is exit
				// 1 is resume
				change = false;

				for (GameObject *g : gameSystem->gameObjects)
				{
					if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
						(y < g->y + (g->length / 2) && y > g->y - (g->length / 2)))
					{
						if (g->id.compare("PauseMenuItem0") == 0)
						{
							// Load main menu
							gameSystem->loadScene(MAIN_MENU);
							change = true;
							break;
						} else if (g->id.compare("PauseMenuItem1") == 0)
						{
							// unload game objects from pause menu
							unloadPauseMenuObjects();
							// continue game
							gameActive = true;
							break;
						}
					}
				}
				if (change)
				{
					msgBus->postMessage(new Msg(LEVEL_LOADED, std::to_string(gameSystem->levelLoaded)), gameSystem);
				}
				break;
			case KEY_ESC_RELEASED:
				// resume gameplay
				// basically button 1
				unloadPauseMenuObjects();
				gameActive = true;
				break;
			default:
				break;
			}
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
			gameSystem->findTankObject(currentObjectId)->shoot(stoi(playerAction[2]), stoi(playerAction[3]));
			playShotSfx(gameSystem->findTankObject(currentObjectId)->getObjectType());
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
						t->updateWorldCoords(60);
					}
				}
			}
			msgBus->postMessage(new Msg(MOVEMENT_SOUND), gameSystem);
		break;
		case PASS:
		{
			std::cout << "Turn passed";
			break;
		}
		//ROTATION
		case ROTATEPOS:
		{
			gameSystem->findTankObject(currentObjectId)->turn(-60,60);
			msgBus->postMessage(new Msg(MOVEMENT_SOUND), gameSystem);
			break;
		}
		case ROTATENEG:
		{
			gameSystem->findTankObject(currentObjectId)->turn(60,60);
			msgBus->postMessage(new Msg(MOVEMENT_SOUND), gameSystem);
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
		validActionsOverlay->renderable = playerTank->shootOverlayRenderable;
		validActionsOverlay->width = playerTank->shootOverlaySize;
		validActionsOverlay->length = playerTank->shootOverlaySize;
		validActionsOverlay->postPostionMsg();

		actionIndicator->renderable = "Reticle.png";
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "shootIcon,1,Reticle.png"), gameSystem);
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "moveIcon,1,moveIconInactive.png"), gameSystem);
		break;
	case MOVE:
		validActionsOverlay->renderable = playerTank->moveOverlayRenderable;
		validActionsOverlay->width = playerTank->moveOverlaySize;
		validActionsOverlay->length = playerTank->moveOverlaySize;
		validActionsOverlay->zRotation = actionOrigin->zRotation;
		validActionsOverlay->postPostionMsg();

		actionIndicator->renderable = "moveIcon2.png";
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "shootIcon,1,ReticleInactive.png"), gameSystem);
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "moveIcon,1,moveIconActive.png"), gameSystem);	
		break;
	case ROTATENEG:
		validActionsOverlay->renderable = "nothing.png";
		validActionsOverlay->postSpriteMsg();
		actionIndicator->renderable = "rotateIconCW.png";
		break;
	case ROTATEPOS:
		validActionsOverlay->renderable = "nothing.png";
		validActionsOverlay->postSpriteMsg();
		actionIndicator->renderable = "rotateIconCCW.png";
		break;
	}
	if (playerTank)
		actionIndicator->zRotation = playerTank->zRotation;

	actionIndicator->postPostionMsg();
	updateReticle();
}

void Scene_Gameplay::updateReticle() {
	gameSystem->reticle->updateWorldCoords(15);
	//gameSystem->sendUpdatePosMessage(gameSystem->reticle);
	if (!playerTank) return; //if the player tank is null return
	updateActionOrigin(actionOrigin);
	checkAOEReticle();
	if (ActionType == SHOOT) {
		moveCost = playerTank->checkShootValidity(validActionsOverlay, gameSystem->reticle->gridX, gameSystem->reticle->gridY);
	}
	else if(ActionType == MOVE)
		moveCost = playerTank->checkMoveValidity(validActionsOverlay, gameSystem->reticle->gridX, gameSystem->reticle->gridY);
	else if (ActionType == ROTATENEG || ActionType == ROTATEPOS) {
		moveCost = playerTank->checkTurnValidity(validActionsOverlay->gridX, actionOrigin->gridY, gameSystem->reticle->gridX, gameSystem->reticle->gridY);
	}
		

	if (moveCost > 0 && gameSystem->reticle->renderable != "TileIndicator.png") {
		gameSystem->reticle->renderable = "TileIndicator.png";
		gameSystem->reticle->postSpriteMsg();
	}	
	else if(moveCost <= 0 && gameSystem->reticle->renderable != "TileIndicatorRed") {
		gameSystem->reticle->renderable = "TileIndicatorRed";
		gameSystem->reticle->postSpriteMsg();
	}
}

void Scene_Gameplay::checkAOEReticle() {
	if (playerTank->getObjectType() == "Tank_Artillery" && ActionType == SHOOT) {
		gameSystem->reticle->length = gameSystem->reticle->originalLength * 4.0f;
		gameSystem->reticle->width = gameSystem->reticle->originalWidth * 4.0f;
		gameSystem->reticle->postSpriteMsg();
	}
	else {
		gameSystem->reticle->length = gameSystem->reticle->originalLength;
		gameSystem->reticle->width = gameSystem->reticle->originalWidth;
		gameSystem->reticle->postSpriteMsg();
	}
};

void Scene_Gameplay::setPlayerTank(std::string playerID) {
	if (playerTank != nullptr) {
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, playerTank->id + ",1,sciFiTank2.png,"), gameSystem);
	}
	for (GameObject* g : gameSystem->gameObjects) {
		if (g->id == playerID)
			if(playerTank = dynamic_cast<TankObject*>(g)) {
				updateActionOrigin(playerTank);
				msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, playerTank->id + ",1,sciFiTank.png,"), gameSystem);

				string debugS = "PLAYER POINTER SET TO: " + playerID + "\n";
				OutputDebugString(debugS.c_str());
		}
	}
}

void Scene_Gameplay::loadPauseMenu() {
	gameSystem->addGameObjects("pause_menu.txt");
}

void Scene_Gameplay::unloadPauseMenuObjects() {
	gameSystem->deleteGameObject(gameSystem->findFullscreenObject("PauseMenuItem0"));
	gameSystem->deleteGameObject(gameSystem->findFullscreenObject("PauseMenuItem1"));
}

void Scene_Gameplay::sendNetworkActionMsg(ActionTypes actionType) {
	ostringstream oss;
	Msg* mm = new Msg(NETWORK_S_ACTION, "");
	oss << gameSystem->clientID << ","//playerID
		<< to_string(actionType) << ","//action type (e.g. MOVE or SHOOT)
									   //<< currentAction << ","//the action number 0 to <number of actions/turn>
		<< gameSystem->reticle->gridX << "," //target x pos
		<< gameSystem->reticle->gridY; //target y pos

	mm->data = oss.str();
	msgBus->postMessage(mm, gameSystem);
}


void Scene_Gameplay::updateActionOrigin(GridObject* newOrigin) {
	if (newOrigin == nullptr) return;

	queuedOrientation = (queuedOrientation + 360) % 360;

	actionOrigin = newOrigin;


	//actionOrigin->zRotation = queuedOrientation;

	validActionsOverlay->zRotation = queuedOrientation;
	validActionsOverlay->gridX = newOrigin->gridX;
	validActionsOverlay->gridY = newOrigin->gridY;
	validActionsOverlay->updateWorldCoords();

	

	
	
}
void Scene_Gameplay::playShotSfx(std::string objectType) {
	if (objectType == "Tank_Scout") {
		msgBus->postMessage(new Msg(REGULAR_SHOT_SOUND), gameSystem);
	}
	else if (objectType == "Tank_Sniper") {
		msgBus->postMessage(new Msg(SNIPER_SHOT_SOUND), gameSystem);
	}
	else if (objectType == "Tank_Heavy") {
		msgBus->postMessage(new Msg(REGULAR_SHOT_SOUND), gameSystem);
	}
	else if (objectType == "Tank_Artillery") {
		msgBus->postMessage(new Msg(ARTILLERY_SHOT_SOUND), gameSystem);
	}

}