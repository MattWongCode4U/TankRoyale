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
			}
			framesSinceTurnStart++;

			displayTimeLeft(30 - ((clock() - turnStartTime) / 1000));
									
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
			Msg* m = new Msg(LEVEL_LOADED, std::to_string(levelLoaded));
			msgBus->postMessage(m, this);
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

	TankObject* player = nullptr;
	for (GameObject* g : gameObjects) {
		if (g->id == "player1" && g->getObjectType() == "TankObject") {
			player = (TankObject*)g;
		}
	}

	if (actionOrigin == nullptr) {
		actionOrigin = player;
	}
	//vector<string> actionsArray;
	vector<string> playersArray;
	vector<string> playerAction;

	Vector2 reticleWorldPos;
	
	
	int dist;
	switch (msg->type) {
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
			oss << player->id << ","//playerID
				<< "MOVE" << ","//action type just MOVE for now
				<< currentAction << ","//the action number 0 to <number of actions/turn>
				<< reticle->x << "," //target x pos
				<< reticle->y; //target y pos

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

			indicator->gridX = reticle->gridX;
			indicator->gridY = reticle->gridY;
			indicator->updateWorldCoords();
			indicator->renderable = "TileIndicatorNum" + to_string(currentAction) + ".png";
			sendUpdatePosMessage(indicator);

			if(ActionType == MOVE)
				actionOrigin = indicator;

			currentAction++;

			break;
		}
		case KEY_A_PRESSED:
			setActionType(MOVE);
			break;

		case KEY_D_PRESSED:
			setActionType(SHOOT);
			break;

		case NETWORK_TURN_BROADCAST:
			actionsToExecute = split(msg->data, '\n');
			//OutputDebugString(actionsToExecute[0].c_str());
	/*		playersArray = split(actionsToExecute[0], ']');
			playerAction = split(playersArray[0], ',');
			player->x = stoi(playerAction[2]);
			player->y = stoi(playerAction[3]);
			sendUpdatePosMessage(player);*/
			currentAction = 0;
			framesSinceTurnStart = 0;
			break;

		case TEST_KEY_PRESSED:
			break;

		case SHOOT_CANNON: 
			break;
	
		case GO_COLLISION:
		/*
		{
			vector<string> data = split(msg->data, ',');

			for (GameObject* g : gameObjects) {
				//OutputDebugString(g->id.c_str());
				if (g->id == data[0]) {
					for (GameObject* o : gameObjects) {
						if (o->id == data[1]) {
							g->onCollide(o);
							break;
						}
					}
				}

			}
			break;
		}
		*/
		case UPDATE_OBJECT_POSITION: {

			vector<string> data = split(msg->data, ',');

			for (GameObject* g : gameObjects) {
				//OutputDebugString(g->id.c_str());

				if (g->id == data[0]) {
					g->x = stof(data[2].c_str());
					g->y = stof(data[3].c_str());
					g->orientation = stoi(data[5].c_str());
				}

			}
			break;
		}
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

	//clear Indicators
	for (GameObject* g : gameObjects) {
		if (g->id.find("TileIndicator") != std::string::npos) {
			g->renderable = "nothing.png";
			//move object out side of camera instead of changing renderable. Temp Solution
			g->x = 1000;
			sendUpdatePosMessage(g);
		}
	}


	vector<string> playerAction;
	vector<string> players = split(actionsToExecute[a], ']');

	for (string s : players) {
		playerAction = split(s, ',');

		//display player actions for players whose id's are found
		for (GameObject* g : gameObjects) {
			if (g->id == playerAction[0]) {
				g->x = stof(playerAction[2]);
				g->y = stof(playerAction[3]);
				sendUpdatePosMessage(g);
			}			
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

//function cube_to_oddr(cube) :
//	col = cube.x + (cube.z - (cube.z & 1)) / 2
//	row = cube.z
//	return Hex(col, row)
//
//	function oddr_to_cube(hex) :
//	x = hex.col - (hex.row - (hex.row & 1)) / 2
//	z = hex.row
//	y = -x - z
//	return Cube(x, y, z)
//
//	return (abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z)) / 2

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
	oss << reticle->id << ",1," << reticle->renderable;
	mm->type = UPDATE_OBJ_SPRITE;
	mm->data = oss.str();
	msgBus->postMessage(mm, this);
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