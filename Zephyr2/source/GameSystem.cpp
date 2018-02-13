#include "GameSystem.h"

GameSystem::GameSystem(MessageBus* mbus) : System(mbus) {
	objData = *(new ObjectData());
}


GameSystem::~GameSystem() {
}

//reads gameobjects from a file. instantiates them and adds them to the list of active objects
void GameSystem::addGameObjects(string fileName) {

	std::string data = openFileFromAsset(fileName, ASSET_TYPE::DATA, true);
	
	vector<string> splitDataVector = split(data, ';');//split gameobjects by

	GameObject* g; //new gameobject to be created
	//loop through objects read in from file
	for (int j = 0; j < splitDataVector.size(); j++) {

		vector<string> splitObjData = split(splitDataVector[j], ',');
		
		std::map<std::string, std::string> gameObjDataMap;
		//loop through elements of each GameObject and add them to the object parameter map
		for (int i = 0; i < splitObjData.size(); i++) {
			vector<string> keyValue = split(splitObjData[i], ':');
			gameObjDataMap[keyValue[0]] = keyValue[1];
		}

		//gets the gameObject type
		string gameObjectType = gameObjDataMap.find("gameObjectType")->second;
		g = NULL;
		//just hard coded else ifs for now... should probably make retreive available classes automatically <- Did some research, cpp doesn't support reflection (Hank)
		if (gameObjectType.compare("GridObject") == 0) {
			g = new GridObject(gameObjDataMap, &objData);
			//OutputDebugString(g->toString().c_str());
		}
		else if (gameObjectType.compare("GameObject") == 0) {
			g = new GameObject(gameObjDataMap, &objData);
		}
		else if (gameObjectType.compare("FullscreenObj") == 0) {
			g = new FullscreenObj(gameObjDataMap, &objData);
		}
		else if (gameObjectType.compare("TankObject") == 0) {
			g = new TankObject(gameObjDataMap, &objData);
		}

		if (g != NULL) {
			createGameObject(g);
		}
	}
}

void GameSystem::saveToFIle(string fileName) {
	string output = "";
	for (GameObject* obj : gameObjects) {
		output += obj->toString();
		output.pop_back();//remove the tailing ','
		output += ";\n";
	}
	writeToFile(fileName, output);
}

// This function adds a created game object to the main list, and posts a message to the render
// and physics systems so that they can add it to their list as well
void GameSystem::createGameObject(GameObject* g) {
	//check if object id already exists
	for (GameObject* obj : gameObjects) {
		if (g->id == obj->id) {
			return;
		}
	}
	gameObjects.push_back(g);
	std::ostringstream oss; //id,renderable,x,y,z,orientation,width,length,physicsEnabled,objectType,imageFrames,renderType,model,normalMap,smoothness
	oss << g->id << ','
		<< g->renderable << ','
		<< g->x << ',' << g->y << ',' << g->z << ','
		<< g->orientation << ','
		<< g->width << ',' << g->length << ','
		//<< g->physicsEnabled << ','
		<< g->getObjectType() << ','
		<< g->imageFrames << ","
		<< (int)g->renderType << ","
		<< g->model << ","
		<< g->normalMap << ","
		<< g->smoothness;
	//<< g->renderable;
	// maybe add the rest of the variables into the oss as well, but can decide later depending on
	// what physics needs

	msgBus->postMessage(new Msg(GO_ADDED, oss.str()), this);
}



void GameSystem::startSystemLoop() {
	//clocks for limiting gameloop speed
	clock_t thisTime = clock();
	
	int enemySpawnCooldownCounter = 0;

	int currentGameTime = 0;
	framesSinceTurnStart = 9999;
	while (alive) {
		thisTime = clock();
		if (thisTime  < currentGameTime) {
			//Sleep(currentGameTime - thisTime);
			std::this_thread::sleep_for(std::chrono::milliseconds(currentGameTime - thisTime));
		}
		currentGameTime = thisTime + timeFrame;
		

		handleMsgQ();

		////Display Thread ID for Debugging
		//std::string s = std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));
		//OutputDebugString("GameSystem Loop on thread: ");
		//OutputDebugString(s.c_str());
		//OutputDebugString("\n");

		/////////////////////////////////////////////////////////////////////
		//							OK to Run							   //
		/////////////////////////////////////////////////////////////////////

		m = new Msg(EMPTY_MESSAGE, "");

		switch (levelLoaded) {
		case -1: // First launch
			// this means we've just started up the system. We should load the main menu
			levelLoaded = 0;
			m = new Msg(LEVEL_LOADED, "0");
			msgBus->postMessage(m, this);

			// Load Main Menu Scene
			addGameObjects("main_menu.txt");
			break;
		case 0: // Menu page
			// does nothing as user changes are handled inside handleMessage. In this state,
			// the only thing we could possibly do is... idk yet.
			break;
		case 1: // Settings page
			// does nothing as user changes are handled inside handleMessage. In this state,
			// the only thing we could possibly do is... idk yet. Basically same shit as
			// the Menu page tho
			break;
		case 2: { // Game loaded
			//execute actions
			//OutputDebugString("\n");
			//OutputDebugString(to_string(framesSinceTurnStart).c_str());
			if (framesSinceTurnStart == 0) {
				//clear Indicators for move selection
				for (GameObject* g : gameObjects) {
					if (g->id.find("TileIndicator") != std::string::npos) {
						g->renderable = "nothing.png";
						//move object out side of camera instead of changing renderable. Temp Solution
						//g->x = 1000;
						//sendUpdatePosMessage(g);
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, g->id + ",1," + g->renderable), this);
					}
				}

				executeAction(0);
				turnStartTime = clock();
			}
			else if (framesSinceTurnStart == 100) {
				executeAction(1);
			}
			else if (framesSinceTurnStart == 200) {
				executeAction(2);
			}
			else if (framesSinceTurnStart == 300) {
				executeAction(3);
				msgBus->postMessage(new Msg(NETWORK_S_ANIMATIONS, ""), this);//tells network system action animation is done on client
				//spam out actions if dead
			}
			else if (framesSinceTurnStart == 350){
				//clear Explosions from previous actions
				for (GameObject* ex : gameObjects) {
					if (ex->id.find("xplosion") != std::string::npos) {
						gameObjects.erase(remove(gameObjects.begin(), gameObjects.end(), ex), gameObjects.end());
						gameObjectRemoved(ex);
					}
				}
			}
			framesSinceTurnStart++;

			//displayTimeLeft(30 - ((clock() - turnStartTime) / 1000));
									
			//OutputDebugString("\n");
			//OutputDebugString(to_string(framesSinceTurnStart/timeFrame).c_str());

			bool endgame = false;

			//getGridDistance(reticle->gridX)

			for (GameObject* obj : gameObjects) {
				obj->earlyUpdate();
			}

			for (GameObject* obj : gameObjects) {
				obj->midUpdate();
			}

			for (GameObject* obj : gameObjects) {
				obj->lateUpdate();
			}

			//loop through list of objects to create added by the gameobjects
			for (GameObject* c : objData.toCreateVector) {
				createGameObject(c);
			}
			objData.toCreateVector.clear();

			//loop through list of objects to destroy added by the gameobjects
			for (GameObject* g : objData.toDestroyVector) {
				gameObjectRemoved(g);

				// increase score
				if (g->id != "playerShip") {
					if (g->getObjectType() == "ShipObj") {
						score++; 
					}
				} 

				// end game
				if (g->id == "playerShip") {
					endgame = true;
				}

				gameObjects.erase(remove(gameObjects.begin(), gameObjects.end(), g), gameObjects.end());
			}
			objData.toDestroyVector.clear();


			//loop through list of messages to send that were added by Game objects
			for (Msg* m : objData.toPostVector) {
				msgBus->postMessage(m, this);
			}
			objData.toPostVector.clear();

			if (endgame) {
				removeAllGameObjects();

				levelLoaded = 3;
				m = new Msg(LEVEL_LOADED, "3");
				msgBus->postMessage(m, this);
				markerPosition = 1;

				// Load Main Menu Scene
				addGameObjects("gameover_menu.txt");

				// add score code here
				// position 0
				int p0 = score / 10;
				// position 1
				int p1 = score % 10;

				std::ostringstream oss;
				Msg* mm = new Msg(UPDATE_OBJ_SPRITE, "");
				oss << "scorepos0,1," << p0 << ".png,";
				mm->data = oss.str();
				msgBus->postMessage(mm, this);

				std::ostringstream osss;
				Msg* m = new Msg(UPDATE_OBJ_SPRITE, "");
				osss << "scorepos1,1," << p1 << ".png,";
				m->data = osss.str();
				msgBus->postMessage(m, this);
				break;
			}
			break;
		}
		default:
			break;
		}
		delete(m);
	}
}

// wrapper that removes all game objects by sending a message for each object.
// the other possible way to do this is by using a functiona nd then posting a specific
// message to the render system, but i don't wanna touch the render system right now
void GameSystem::removeAllGameObjects() {
	for (GameObject* go : gameObjects) {
		gameObjectRemoved(go);
		//gameObjects.erase(remove(gameObjects.begin(), gameObjects.end(), go), gameObjects.end());
	}

	gameObjects.clear();
}

void GameSystem::gameObjectRemoved(GameObject* g) {
	Msg* m = new Msg(GO_REMOVED, g->id);
	msgBus->postMessage(m, this);
}

void GameSystem::handleMessage(Msg *msg) {

	// call the parent first 
	System::handleMessage(msg);

	//selects loaded level and calls the message handler for that level
	switch (levelLoaded) {
	case 0:
		mainMenuHandler(msg);
		break;
	case 1:
		settingsMenuHandler(msg);
		break;
	case 2:
		lvl1Handler(msg);
		break;
	case 3:
		gameOverMenuHandler(msg);
		break;
	case 4:
		instructionMenuHandler(msg);
		break;
	default:
		break;
	}
}

//handles messages when in the main menu screen
void GameSystem::mainMenuHandler(Msg * msg) {
	std::ostringstream oss;
	Msg* mm = new Msg(EMPTY_MESSAGE, "");
	//GameObject* g;

	switch (msg->type) {
	case LEFT_MOUSE_BUTTON:
	{
		vector<string> objectData = split(msg->data, ',');
		INT32 x = atoi(objectData[0].c_str());
		INT32 y = atoi(objectData[1].c_str());
		INT32 width = atoi(objectData[2].c_str());
		INT32 length = atoi(objectData[3].c_str());
		x -= width / 2; y -= length / 2;
		y = -y;
		bool change = false;


		for (GameObject *g : gameObjects)
		{
			if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
				(y < g->y + (g->length / 2) && y > g->y - (g->length / 2)))
			{
				if (g->id.compare("Menu_Item1") == 0)
				{
					// instructions page
					removeAllGameObjects();
					addGameObjects("instructions_menu.txt");
					levelLoaded = 4;
					markerPosition = 0;
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item2") == 0)
				{
					// start the game (or go to level select?)
					// first, clear all objects
					removeAllGameObjects();

					// then, load new objects
					//addGameObjects("Level_1.txt");
					addGameObjects("prototype_level.txt");
					levelLoaded = 2;
					score = 0;
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item3") == 0)
				{
					// Go to settings
					removeAllGameObjects();
					addGameObjects("settings_menu.txt");
					levelLoaded = 1;
					markerPosition = 0;
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item4") == 0)
				{
					malive = false;
					break;
				}
			}
		}
		if (change) 
		{
			msgBus->postMessage(new Msg(LEVEL_LOADED, std::to_string(levelLoaded)), this);
			msgBus->postMessage(new Msg(READY_TO_START_GAME, ""), this);
			setPlayerTank("player1");
		}

		break;
	}
	case MOUSE_MOVE:
	{
		vector<string> objectData = split(msg->data, ',');
		INT32 x = atoi(objectData[0].c_str());
		INT32 y = atoi(objectData[1].c_str());
		INT32 width = atoi(objectData[2].c_str());
		INT32 length = atoi(objectData[3].c_str());
		x -= width / 2; y -= length / 2;
		y = -y;
		bool change = false;


		for (GameObject *g : gameObjects)
		{
			if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
				(y < g->y + (g->length / 2) && y > g->y - (g->length / 2)))
			{
				if (g->id.compare("Menu_Item1") == 0 && markerPosition != 0)
				{
					markerPosition = 0; change = true;
				}
				else if (g->id.compare("Menu_Item2") == 0 && markerPosition != 1)
				{
					markerPosition = 1; change = true;
				} 
				else if (g->id.compare("Menu_Item3") == 0 && markerPosition != 2)
				{
					markerPosition = 2; change = true;
				}
				else if (g->id.compare("Menu_Item4") == 0 && markerPosition != 3)
				{
					markerPosition = 3; change = true;
				}
			}
		}
		if (change) {
			mm->type = UPDATE_OBJ_SPRITE;
			oss << "MarkerObj,1,MZ6_Marker_P" << markerPosition << ".png,";
			mm->data = oss.str();
			msgBus->postMessage(mm, this);
		}
		break;
	}
	case DOWN_ARROW_PRESSED:
		// move the marker location and let rendering know?
		markerPosition++;
		if (markerPosition > 3) {
			markerPosition = 3;
		}

		mm->type = UPDATE_OBJ_SPRITE;
		oss << "MarkerObj,1,MZ6_Marker_P" << markerPosition << ".png,";
		mm->data = oss.str();
		msgBus->postMessage(mm, this);
		break;
	case UP_ARROW_PRESSED:
		// move the marker location and let rendering know?
		markerPosition--;
		if (markerPosition < 0) {
			markerPosition = 0;
		}
		markerPosition = markerPosition % 4;

		mm->type = UPDATE_OBJ_SPRITE;
		oss << "MarkerObj,1,MZ6_Marker_P" << markerPosition << ".png,";
		mm->data = oss.str();
		msgBus->postMessage(mm, this);
		break;
	case SPACEBAR_PRESSED:
		if (markerPosition == 3) {
			// Exit was selected, kill main
			malive = false;
		}
		else if (markerPosition == 2) {
			// Go to settings
			removeAllGameObjects();
			addGameObjects("settings_menu.txt");
			levelLoaded = 1;
			markerPosition = 0;
			Msg* m = new Msg(LEVEL_LOADED, "1");
			msgBus->postMessage(m, this);
		}
		else if (markerPosition == 1) {
			// start the game (or go to level select?)
			// first, clear all objects
			removeAllGameObjects();

			// then, load new objects
			//addGameObjects("Level_1.txt");
			addGameObjects("prototype_level.txt");
			levelLoaded = 2;
			Msg* m = new Msg(LEVEL_LOADED, "2");
			msgBus->postMessage(m, this);
			score = 0;
			//let NetworkSystem know client is ready to start game
			msgBus->postMessage(new Msg(READY_TO_START_GAME, ""), this);
			OutputDebugString("SENDING READY_TO_START_GAME MSG");

			setPlayerTank("player1");
		}
		else if (markerPosition == 0) {
			// instructions page
			removeAllGameObjects();
			addGameObjects("instructions_menu.txt");
			levelLoaded = 4;
			markerPosition = 0;
			Msg* m = new Msg(LEVEL_LOADED, "4");
			msgBus->postMessage(m, this);
		}
		break;

	case NETWORK_CONNECT:
		clientID = msg->data;
		OutputDebugString("GS: NETWORK_CONNECT RECEIVED: \n");
		OutputDebugString(clientID.c_str());
		OutputDebugString("\n");
		break;
	default:
		break;
	}
}

//message handling when in the instructions screen
void GameSystem::instructionMenuHandler(Msg * msg) {
	// only one option; to go back to menu
	if (msg->type == SPACEBAR_PRESSED) {
		removeAllGameObjects();
		addGameObjects("main_menu.txt");
		levelLoaded = 0;
		markerPosition = 0;
		Msg* m = new Msg(LEVEL_LOADED, "0");
		msgBus->postMessage(m, this);
	}
}

//the settings menu message handler
void GameSystem::settingsMenuHandler(Msg * msg) {
	std::ostringstream oss;
	Msg* mm = new Msg(EMPTY_MESSAGE, "");
	switch (msg->type) {
	case DOWN_ARROW_PRESSED:
		// move the marker location and let rendering know?
		markerPosition++;
		if (markerPosition > 2) {
			markerPosition = 2;
		}

		mm->type = UPDATE_OBJ_SPRITE;
		oss << "obj3,1,Z6_Marker_P" << markerPosition << ".png,";
		mm->data = oss.str();
		msgBus->postMessage(mm, this);
		break;
	case UP_ARROW_PRESSED:
		// move the marker location and let rendering know?
		markerPosition--;
		if (markerPosition < 0) {
			markerPosition = 0;
		}
		markerPosition = markerPosition % 3;

		mm->type = UPDATE_OBJ_SPRITE;
		oss << "obj3,1,Z6_Marker_P" << markerPosition << ".png,";
		mm->data = oss.str();
		msgBus->postMessage(mm, this);
		break;
	case SPACEBAR_PRESSED:
		if (markerPosition == 2) {
			// Back button, go to menu
			removeAllGameObjects();
			addGameObjects("main_menu.txt");
			levelLoaded = 0;
			markerPosition = 0;
			Msg* m = new Msg(LEVEL_LOADED, "0");
			msgBus->postMessage(m, this);
		}
		else if (markerPosition == 1) {
			// change game sound to "off"
			mm->type = AUDIO_MUTE;
			mm->data = "1";
			msgBus->postMessage(mm, this);
		}
		else if (markerPosition == 0) {
			// change game sound to "on"
			mm->type = AUDIO_MUTE;
			mm->data = "0";
			msgBus->postMessage(mm, this);
		}
		break;
	default:
		break;
	}
}

//handles messages in level1 scene
void GameSystem::lvl1Handler(Msg * msg) {
	std::ostringstream oss;
	Msg* mm = new Msg(EMPTY_MESSAGE, "");
	//GameObject* g;

	//GridObject* reticle = nullptr;
	for (GameObject* g : gameObjects) {
		if (g->id == "reticle"&& g->getObjectType() =="GridObject") {
			reticle = (GridObject*)g;
		}
	}

	//TankObject* player = nullptr;


	//vector<string> actionsArray;
	vector<string> playersArray;
	vector<string> playerAction;

	Vector2 reticleWorldPos;

	int dist;

	//messages to be read in both active and inactive game state
	switch (msg->type) {
	case NETWORK_R_START_TURN:
		gameActive = true;
		OutputDebugString("RECEVIED NETWORK_R_START_TURN... INPUT UNBLOCKED\n");
		actionOrigin = playerTank;

		if (playerTank->health <= 0) {
			string spoofData = clientID + ",3,0,0";
			msgBus->postMessage(new Msg(NETWORK_S_ACTION, spoofData), this);
			msgBus->postMessage(new Msg(NETWORK_S_ACTION, spoofData), this);
			msgBus->postMessage(new Msg(NETWORK_S_ACTION, spoofData), this);
			msgBus->postMessage(new Msg(NETWORK_S_ACTION, spoofData), this);
			currentAction = maxActions;
		}

		updateReticle();
		break;

	case NETWORK_TURN_BROADCAST:
		actionsToExecute = split(msg->data, '\n');
		currentAction = 0;
		framesSinceTurnStart = 0;
		gameActive = false;
		OutputDebugString("RECEVIED NETWORK_TURN_BROADCAST... INPUT BLOCKED\n");
		break;
	case NETWORK_R_GAMESTART_OK: {//"id1,ClientID,id3,id4," only for server version
		vector<string> clientIDVector = split(msg->data, ',');
		//OutputDebugString(to_string(clientIDVector.size()).c_str());
		
		OutputDebugString("GS: NETWORK_R_GAMESTART_OK RECEIVED:  ");
		OutputDebugString(msg->data.c_str());
		OutputDebugString("\n");

		for (int i = 0; i < clientIDVector.size(); i++) {
			if (clientIDVector[i] == clientID) {		
				//set the new player tank
				setPlayerTank("player" + to_string(i + 1));			
			}
		}
		break;
	}
	case NETWORK_R_PING:
		displayTimeLeft(stoi(msg->data));
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
			x -= width / 2; y -= length / 2;
			y = -y;

			int offsetX = x;
			int offsetY = y;

			if (y < -44) offsetY -= 45;
			else if (y > 44) offsetY += 45;

			int gridLocationY = (offsetY * 2 / 3) / hexSize;

			if (x < -44 && gridLocationY % 2 == 0) offsetX -= 45;
			else if (x > 44 && gridLocationY % 2 == 0) offsetX += 45;
			else if (x <= 0 && gridLocationY % 2 != 0) offsetX -= 90;

			int gridLocationX = (offsetX * sqrt(3) / 3) / hexSize;

		    reticle->gridX = gridLocationX;
			reticle->gridY = gridLocationY;
			updateReticle();

			break;
		}
		case DOWN_ARROW_PRESSED: {
			reticle->gridY--;
			updateReticle();
			break;
		}
		case UP_ARROW_PRESSED:
			reticle->gridY++;
			updateReticle();
			break;

		case LEFT_ARROW_PRESSED:
			reticle->gridX--;
			updateReticle();
			break;

		case RIGHT_ARROW_PRESSED:
			reticle->gridX++;
			updateReticle();
			break;

		case SPACEBAR_PRESSED: {
			if (currentAction >= maxActions || !validMove) break;
				
			//Send Message to network
			//message format: playerID,actionName,actionNumber,targetX,targetY
			oss << clientID << ","//playerID
				<< to_string(ActionType) << ","//action type (e.g. MOVE or SHOOT)
				//<< currentAction << ","//the action number 0 to <number of actions/turn>
				<< reticle->gridX << "," //target x pos
				<< reticle->gridY; //target y pos

			mm->type = NETWORK_S_ACTION;
			mm->data = oss.str();
			msgBus->postMessage(mm, this);

			GridObject* indicator = NULL;

			string indicatorId = "TileIndicator" + to_string(currentAction);

			for (GameObject* g : gameObjects) {
				if (g->id == indicatorId) {
					indicator = (GridObject*)g;
				}
			}

			if (ActionType == MOVE) {
				actionOrigin = indicator;
				indicator->renderable = "MoveIndicator.png";
			}
			else if (ActionType == SHOOT) {
				indicator->renderable = "ShootIndicator.png";
			}


			indicator->gridX = reticle->gridX;
			indicator->gridY = reticle->gridY;
			indicator->updateWorldCoords();
			//indicator->renderable = "TileIndicatorNum" + to_string(currentAction) + ".png";
			sendUpdatePosMessage(indicator);//send indicator position message

			//send update sprite message. maybe this sould be included in update position?
			msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, indicator->id + ",1," + indicator->renderable), this);


			currentAction++;

			break;
		}
		case KEY_A_PRESSED:
			(ActionType == MOVE) ? setActionType(SHOOT) : setActionType(MOVE);
			updateReticle();
			break;

		case KEY_D_PRESSED: {
			
			

			//return the position of the reticle for debugging purposes
			string s = "RETICLE AT GRID("
				+ to_string(reticle->gridX)
				+ "," + to_string(reticle->gridY)
				+ ")  WORLD("
				+ to_string(reticle->x)
				+ "," + to_string(reticle->y)
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

			for (GameObject* g : gameObjects) {

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

//gameover menu message handler
void GameSystem::gameOverMenuHandler(Msg * msg) {
	std::ostringstream oss;
	Msg* mm = new Msg(EMPTY_MESSAGE, "");
	switch (msg->type) {
	case DOWN_ARROW_PRESSED:
		// move the marker location and let rendering know?
		markerPosition++;
		if (markerPosition > 2) {
			markerPosition = 2;
		}

		mm->type = UPDATE_OBJ_SPRITE;
		oss << "obj3,1,Z6_Marker_P" << markerPosition << ".png,";
		mm->data = oss.str();
		msgBus->postMessage(mm, this);
		break;
	case UP_ARROW_PRESSED:
		// move the marker location and let rendering know?
		markerPosition--;
		if (markerPosition < 1) {
			markerPosition = 1;
		}
		markerPosition = markerPosition % 3;

		mm->type = UPDATE_OBJ_SPRITE;
		oss << "obj3,1,Z6_Marker_P" << markerPosition << ".png,";
		mm->data = oss.str();
		msgBus->postMessage(mm, this);
		break;
	case SPACEBAR_PRESSED:
		// End Game Screen
		if (markerPosition == 2) {
			// go to menu
			removeAllGameObjects();
			addGameObjects("main_menu.txt");
			levelLoaded = 0;
			markerPosition = 0;
			Msg* m = new Msg(LEVEL_LOADED, "0");
			msgBus->postMessage(m, this);
		}
		else if (markerPosition == 1) {
			// start the game (or go to level select?)
			// first, clear all objects
			removeAllGameObjects();

			// then, load new objects
			addGameObjects("prototype_level.txt"); // TEMPORARY 
			levelLoaded = 2;
			Msg* m = new Msg(LEVEL_LOADED, "2");
			msgBus->postMessage(m, this);
			score = 0;

			//set the default player tank
			setPlayerTank("player1");

			//let NetworkSystem know client is ready to start game
			msgBus->postMessage(new Msg(READY_TO_START_GAME, ""), this);

		}
	default:
		break;
	}
}

void GameSystem::sendUpdatePosMessage(GameObject* g) {
	std::ostringstream oss;
	Msg* mm = new Msg(EMPTY_MESSAGE, "");

	//UPDATE_OBJECT_POSITION, //id,renderable,x,y,z,orientation,width,length,type
	oss << g->id << ","
		<< g->renderable << ","
		<< g->x << ","
		<< g->y << ","
		<< g->z << ","
		<< g->orientation << ","
		<< g->width << ","
		<< g->length << ","
		//<< g->physicsEnabled << ","
		<< g->getObjectType();

	mm->type = UPDATE_OBJECT_POSITION;
	mm->data = oss.str();
	msgBus->postMessage(mm, this);
}

//execute the actions received from the network
void GameSystem::executeAction(int a) {
	//clear Explosions from previous actions
	for (GameObject* ex : gameObjects) {
		if (ex->id.find("xplosion") != std::string::npos) {
			gameObjects.erase(remove(gameObjects.begin(), gameObjects.end(), ex), gameObjects.end());
			gameObjectRemoved(ex);
		}
	}

	vector<string> playerAction;
	vector<string> players = split(actionsToExecute[a], ']');

	//for (string s : players) {
	for(int playerNum = 0; playerNum < 4; playerNum++){

		playerAction = split(players[playerNum], ',');
		string currentObjectId ="player" + to_string(playerNum + 1);//get id from the order of incoming actions
		ActionTypes receivedAction = static_cast<ActionTypes>(stoi(playerAction[1]));//parse action type

		//switch on the action type received from the network system, and execute the action
		switch (receivedAction) {
		case SHOOT: {
			string newID = "explosion" + to_string(rand());
			GridObject* gr = new GridObject(newID, "explosion.png", 0, 0, 4, 0, 250, 250, 1, stoi(playerAction[2]), stoi(playerAction[3]));
			gr->updateWorldCoords();
			createGameObject(gr);
			dealAOEDamage(stoi(playerAction[2]), stoi(playerAction[3]), 2, 70);
			break;
		}

		case MOVE:
			//display player MOVE actions for players whose id's are found
			for (GameObject* g : gameObjects) {
				if (g->id == currentObjectId) {
					//OutputDebugString(playerAction[2].c_str());
					TankObject* t = (TankObject*)g;
					if (t->health > 0) {
						t->gridX = stoi(playerAction[2]);
						t->gridY = stoi(playerAction[3]);
						t->updateWorldCoords();
						sendUpdatePosMessage(t);
					}
					
				}
			}
			break;
		}
	}
}


//converts grid coordinates to world coordinates
Vector2 GameSystem::gridToWorldCoord(int gridX, int gridY) {
	float hexHeight = hexSize * 2.0f; //height of a single hex tile
	float vertDist = hexHeight * 3.0f / 4.0f;//verticle distance between tile center points
	float hexWidth = sqrt(3.0f) / 2.0f * hexHeight;//width of a single tile. Also the horizontal distance bewteen 2 tiles

	Vector2 worldPos;

	worldPos.x = hexWidth * gridX;
	worldPos.y = vertDist * gridY;
	if (gridY % 2 != 0) 
		worldPos.x += hexWidth / 2;

	return worldPos;
}

int GameSystem::getGridDistance(int aX, int aY, int bX, int bY) {
	int aXCube = aX - (aY - (aY & 1)) / 2;
	int aZCube = aY;
	int aYCube = -aXCube - aZCube;

	int bXCube = bX - (bY - (bY & 1)) / 2;
	int bZCube = bY;
	int bYCube = -bXCube - bZCube;

	return (abs(aXCube - bXCube) + abs(aYCube - bYCube) + abs(aZCube - bZCube)) / 2;
}

void GameSystem::updateReticle() {
	reticle->updateWorldCoords();
	sendUpdatePosMessage(reticle);

	int dist = getGridDistance(reticle->gridX, reticle->gridY, actionOrigin->gridX, actionOrigin->gridY);

	if (dist > range) {
		reticle->renderable = "TileIndicatorRed.png";
		validMove = false;
	}
	else {
		reticle->renderable = "TileIndicator.png";
		validMove = true;
	}

	std::ostringstream oss;
	Msg* mm = new Msg(EMPTY_MESSAGE, "");

	//UPDATE_OBJECT_SPRITE, //id,#Frames,Renderable
	msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, reticle->id + ",1," + reticle->renderable), this);
}

void GameSystem::displayTimeLeft(int time) {
	int p0, p1;
	if (time < 0) {
		p0 = 0;
		p1 = 0;
	}
	else {
		p0 = time % 100;
		p0 /= 10;
		p1 = time % 10;
	}

	std::ostringstream oss;
	Msg* mm = new Msg(UPDATE_OBJ_SPRITE, "");
	oss << "timeLeftpos0,1," << p0 << ".png,";
	mm->data = oss.str();
	msgBus->postMessage(mm, this);

	std::ostringstream osss;
	Msg* m = new Msg(UPDATE_OBJ_SPRITE, "");
	osss << "timeLeftpos1,1," << p1 << ".png,";
	m->data = osss.str();
	msgBus->postMessage(m, this);
}

void GameSystem::setActionType(ActionTypes a) {
	ActionType = a;
	std::ostringstream oss;
	Msg* mm;
	
	switch (a) {
	case SHOOT:
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "shootIcon,1,Reticle.png"), this);
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "moveIcon,1,moveIconInactive.png"), this);
		range = 5;
		break;
	case MOVE:
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "shootIcon,1,ReticleInactive.png"), this);
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "moveIcon,1,moveIconActive.png"), this);
		range = 1;
		break;
	}
}

/*
	Takes in the player number we are going to be udpating. Enum in GameSystem
*/
void GameSystem::updatePlayerHealthBar(string playerID) {
	Msg* m;
	TankObject* curPlayer = nullptr;
	FullscreenObj* curHealthBar = nullptr;
	curPlayer = findTankObject(playerID);
	curHealthBar = findFullscreenObject(playerID + "_hpbar");
	if (curPlayer != nullptr && curHealthBar != nullptr) {
		int hpBarSize = curHealthBar->originalWidth * (1 - (TANK_MAX_HEALTH - curPlayer->getHealth())); // TEST: Does this update the size correctly?
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
		} else if (curPlayer->getHealth() <= 30) {
			std::ostringstream oss;
			// change sprite
			if (curHealthBar->renderable != "red_hpbar")
			{
				oss << curHealthBar->id << ",";
				oss << " ,";
				oss << "red_hpbar.png";
				m = new Msg(MSG_TYPE::UPDATE_OBJ_SPRITE, oss.str());
				msgBus->postMessage(m, this);
			}
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
		} else if (curPlayer->getHealth() <= 50) {
			std::ostringstream oss;
			if (curHealthBar->renderable != "orange_hpbar.png")
			{
				oss << curHealthBar->id << ",";
				oss << curHealthBar->id << ",";
				oss << "orange_hpbar.png";
				m = new Msg(MSG_TYPE::UPDATE_OBJ_SPRITE, oss.str());
				msgBus->postMessage(m, this);
			}
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
		} else { 
			std::ostringstream oss;
			if (curHealthBar->renderable != "green_hpbar.png")
			{
				oss << curHealthBar->id << ",";
				oss << " ,";
				oss << "green_hpbar.png";
				m = new Msg(MSG_TYPE::UPDATE_OBJ_SPRITE, oss.str());
				msgBus->postMessage(m, this);
			}
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
		msgBus->postMessage(m, this);
	}
};

GameObject* GameSystem::findGameObject(std::string objectID) {
	GameObject* obj = nullptr;
	for (GameObject* g : gameObjects) {
		if (g->id == objectID && g->getObjectType() == "GameObject") {
			obj = (GameObject*)g;
			return obj;
		}
	}
	return obj;
}
TankObject* GameSystem::findTankObject(std::string objectID) {
	TankObject* obj = nullptr;
	for (GameObject *g : gameObjects) {
		if (g->id == objectID && g->getObjectType() == "TankObject") {
			obj = (TankObject*)g;
			return obj;
		}
	}
	return obj;
}
GridObject* GameSystem::findGridObject(std::string objectID) {
	GridObject* obj = nullptr;
	for (GameObject* g : gameObjects) {
		if (g->id == objectID && g->getObjectType() == "GridObject") {
			obj = (GridObject*)g;
			return obj;
		}
	}
	return obj;
}
FullscreenObj* GameSystem::findFullscreenObject(std::string objectID) {
	FullscreenObj* obj = nullptr;
	for (GameObject* g : gameObjects) {
		if (g->id == objectID && g->getObjectType() == "FullscreenObj") {
			obj = (FullscreenObj*)g;
			return obj;
		}
	}
	return obj;
}

void GameSystem::dealAOEDamage(int _originX, int _originY, int affectedRadius, int damage) {
	int aXCube = _originX - (_originY - (_originY & 1)) / 2;
	int aZCube = _originY;
	int aYCube = -aXCube - aZCube;
	OutputDebugString("origin point of shot: ");
	OutputDebugString(to_string(_originX).c_str());
	OutputDebugString(" , ");
	OutputDebugString(to_string(_originY).c_str());
	OutputDebugString("\n");

	for (GameObject *go : gameObjects) { //look through all gameobjects
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

				if(tank->health <= 0)
					msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, tank->id + ",1,crater.png,"), this);
				updatePlayerHealthBar(tank->id);
			}
		}
	}
}
void GameSystem::setPlayerTank(std::string playerID) {
	if (playerTank != nullptr) {
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, playerTank->id + ",1,sciFiTank2.png,"), this);
	}
	for (GameObject* g : gameObjects) {
		if (g->id == playerID && g->getObjectType() == "TankObject") {
			playerTank = (TankObject*)g;
			actionOrigin = playerTank;
			msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, playerTank->id + ",1,sciFiTank.png,"), this);

			string debugS = "PLAYER POINTER SET TO: " + playerID + "\n";
			OutputDebugString(debugS.c_str());
		}
	}
}
