#include "GameSystem.h"

GameSystem::GameSystem(MessageBus* mbus) : System(mbus) {
}


GameSystem::~GameSystem() {
}

//creates a gameobject from a file and returns a pointer to it
//DOES NOT ADD OBJECT TO gameobjects vector
GameObject* GameSystem::makeGameObject(string fileName) {
	std::string data = openFileFromAsset(fileName, ASSET_TYPE::DATA, true);
	vector<string> splitObjData = split(data, ',');

	std::map<std::string, std::string> gameObjDataMap;
	//loop through elements of each GameObject and add them to the object parameter map
	for (int i = 0; i < splitObjData.size(); i++) {
		vector<string> keyValue = split(splitObjData[i], ':');
		gameObjDataMap[keyValue[0]] = keyValue[1];
	}
	GameObject* g = NULL;
	//gets the gameObject type
	string gameObjectType = gameObjDataMap.find("gameObjectType")->second;
	
	//just hard coded else ifs for now... should probably make retreive available classes automatically <- Did some research, cpp doesn't support reflection (Hank)
	if (gameObjectType.compare("GridObject") == 0) {
		g = new GridObject(gameObjDataMap, this);
		//OutputDebugString(g->toString().c_str());
	}
	else if (gameObjectType.compare("GameObject") == 0) {
		g = new GameObject(gameObjDataMap, this);
	}
	else if (gameObjectType.compare("FullscreenObj") == 0) {
		g = new FullscreenObj(gameObjDataMap, this);
	}
	else if (gameObjectType.compare("TankObject") == 0) {
		g = new TankObject(gameObjDataMap, this);
	}
	return g;
}

//reads gameobjects from a file. instantiates them and adds them to the list of active objects
void GameSystem::addGameObjects(string fileName) {

	std::string data = openFileFromAsset(fileName, ASSET_TYPE::DATA, true);
	
	vector<string> splitDataVector = split(data, ';');//split gameobjects by ;

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
			g = new GridObject(gameObjDataMap, this);
			//OutputDebugString(g->toString().c_str());
		}
		else if (gameObjectType.compare("GameObject") == 0) {
			g = new GameObject(gameObjDataMap, this);
		}
		else if (gameObjectType.compare("FullscreenObj") == 0) {
			g = new FullscreenObj(gameObjDataMap, this);
		}
		else if (gameObjectType.compare("TankObject") == 0) {
			g = new TankObject(gameObjDataMap, this);
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
	std::ostringstream oss; //id,renderable,x,y,z,orientation,width,length,height,physicsEnabled,objectType,imageFrames,renderType,model,normalMap,smoothness, animationDelay, animateOnce
	oss << g->id << ','
		<< g->renderable << ','
		<< g->x << ',' << g->y << ',' << g->z << ','
		<< g->xRotation << ","
		<< g->yRotation << ","
		<< g->zRotation << ","
		<< g->width << ',' << g->length << ',' << g->height << ','
		//<< g->physicsEnabled << ','
		<< g->getObjectType() << ','
		<< g->imageFrames << ","
		<< (int)g->renderType << ","
		<< g->model << ","
		<< g->normalMap << ","
		<< g->smoothness << ","
		<< g->animationDelay << ","
		<< (int)g->animateOnce;
	//<< g->renderable;
	// maybe add the rest of the variables into the oss as well, but can decide later depending on
	// what physics needs

	msgBus->postMessage(new Msg(GO_ADDED, oss.str()), this);

	//set the object's parent
	if (g->parentId != "") {
		for (GameObject* p : gameObjects) {
			if (p->id == g->parentId) {
				g->setParent(p);
				//g->offsetPosition(p->x, p->y, p->z, p->zRotation);
				//g->offsetPosition(0, 0, 0, 0);
			}
		}
	}
}



void GameSystem::startSystemLoop() {
	//clocks for limiting gameloop speed
	clock_t thisTime = clock();

	int enemySpawnCooldownCounter = 0;

	int currentGameTime = 0;
	while (alive) {
		thisTime = clock();
		if (thisTime < currentGameTime) {
			//Sleep(currentGameTime - thisTime);
			std::this_thread::sleep_for(std::chrono::milliseconds(currentGameTime - thisTime));
		}
		currentGameTime = thisTime + timeFrame;


		handleMsgQ();

		/////////////////////////////////////////////////////////////////////
		//							OK to Run							   //
		/////////////////////////////////////////////////////////////////////

		m = new Msg(EMPTY_MESSAGE, "");

		//if there is no scene loaded, load the main menu
		if (scene == nullptr) {
			loadScene(MAIN_MENU);
		}

		//call the update function on the currently loaded scene
		scene->sceneUpdate();


		//Handle object update methods
		for (GameObject* obj : gameObjects) {
			obj->earlyUpdate();
		}

		for (GameObject* obj : gameObjects) {
			obj->midUpdate();
		}

		for (GameObject* obj : gameObjects) {
			obj->lateUpdate();
		}
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

// Delete game objects and removes them from the renderer
void GameSystem::deleteGameObject(string id) {
	if (findGameObject(id) != nullptr) {
		gameObjectRemoved(findGameObject(id));
		gameObjects.erase(remove(gameObjects.begin(), gameObjects.end(), findGameObject(id)), gameObjects.end());
	}
	else if (findFullscreenObject(id) != nullptr) {
		gameObjectRemoved(findFullscreenObject(id));
		gameObjects.erase(remove(gameObjects.begin(), gameObjects.end(), findFullscreenObject(id)), gameObjects.end());
	}
	else if (findGridObject(id) != nullptr) {
		gameObjectRemoved(findGridObject(id));
		gameObjects.erase(remove(gameObjects.begin(), gameObjects.end(), findGridObject(id)), gameObjects.end());
	}
	else if (findTankObject(id) != nullptr) {
		gameObjectRemoved(findTankObject(id));
		gameObjects.erase(remove(gameObjects.begin(), gameObjects.end(), findTankObject(id)), gameObjects.end());
	}
}

// Delete game objects and removes them from the renderer
void GameSystem::deleteGameObject(GameObject* go) {
	for (GameObject* g : gameObjects) {
		if (g == go) {
			gameObjectRemoved(g);
			gameObjects.erase(remove(gameObjects.begin(), gameObjects.end(), g), gameObjects.end());
		}
	}
}

// Sends message to render system to remove the game object
void GameSystem::gameObjectRemoved(GameObject* g) {
	Msg* m = new Msg(GO_REMOVED, g->id);
	msgBus->postMessage(m, this);
}

void GameSystem::handleMessage(Msg *msg) {

	// call the parent first 
	System::handleMessage(msg);
	
	//call the message handle on the currently loaded scene
	scene->sceneHandleMessage(msg);
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
		<< g->xRotation << ","
		<< g->yRotation << ","
		<< g->zRotation << ","
		<< g->width << ","
		<< g->length << ","
		<< g->height << ","
		//<< g->physicsEnabled << ","
		<< g->getObjectType();

	mm->type = UPDATE_OBJECT_POSITION;
	mm->data = oss.str();
	msgBus->postMessage(mm, this);
}

//converts grid coordinates to world coordinates
//Vector2 GameSystem::gridToWorldCoord(int gridX, int gridY) {
//	float hexHeight = hexSize * 2.0f; //height of a single hex tile
//	float vertDist = hexHeight * 3.0f / 4.0f;//verticle distance between tile center points
//	float hexWidth = sqrt(3.0f) / 2.0f * hexHeight;//width of a single tile. Also the horizontal distance bewteen 2 tiles
//
//	Vector2 worldPos;
//
//	worldPos.x = hexWidth * gridX;
//	worldPos.y = vertDist * gridY;
//	if (gridY % 2 != 0) 
//		worldPos.x += hexWidth / 2;
//
//	return worldPos;
//}

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
	TankObject* tank = nullptr;
	for (GameObject *g : gameObjects) {
		if(g->id == objectID)
			if (TankObject* tank = dynamic_cast<TankObject*>(g)){
				return tank;
			}
	}
	return nullptr;
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


void  GameSystem::loadScene(SceneType _scene){
	
	//calling destructor on the old scene just in case. 
	if(scene!= nullptr)
		scene->~Scene();

	//clear all currently loaded gameObjects
	removeAllGameObjects();

	//instantiate the new scene
	switch (_scene) {
	case MAIN_MENU:
		scene = new Scene_MainMenu(msgBus, this);
		break;
	case LOBBY_MENU:
		scene = new Scene_Lobby(msgBus, this);
		break;
	case GAMEPLAY:
		scene = new Scene_Gameplay(msgBus, this);
		break;
	case SETTINGS_MENU:
		scene = new Scene_SettingsMenu(msgBus, this);
		break;
	case INSTRUCTION_MENU:
		scene = new Scene_InstructionsMenu(msgBus, this);
		break;
	}

	scene->startScene();
}

//post message on the message bus
//implementation of gameSystemUtil virtual function
//used to allow Gameobjects to send messages without giving them full access to GameSystem and messageSystem
void GameSystem::postMessageToBus(Msg* message) {
	msgBus->postMessage(message, this);
}

std::vector<GameObject*>* GameSystem::getGameObjectsVector() {
	return &gameObjects;
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