#include "GameSystem.h"

GameSystem::GameSystem(MessageBus* mbus) : System(mbus) {
	//creates the root for the collision quad tree
	quadTreeRoot = new quadTreeNode(0, 0, 200);
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
		g = new Tank_Artillery(gameObjDataMap, this);
	}
	else if (gameObjectType.compare("Tank_Artillery") == 0) {
		g = new Tank_Artillery(gameObjDataMap, this);
	}
	else if (gameObjectType.compare("Tank_Heavy") == 0) {
		g = new Tank_Heavy(gameObjDataMap, this);
	}
	else if (gameObjectType.compare("Tank_Scout") == 0) {
		g = new Tank_Scout(gameObjDataMap, this);
	}
	else if (gameObjectType.compare("Tank_Sniper") == 0) {
		g = new Tank_Sniper(gameObjDataMap, this);
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
			g = new Tank_Artillery(gameObjDataMap, this);
		}
		else if (gameObjectType.compare("Tank_Artillery") == 0) {
			g = new Tank_Artillery(gameObjDataMap, this);
		}
		else if (gameObjectType.compare("Tank_Heavy") == 0) {
			g = new Tank_Heavy(gameObjDataMap, this);
		}
		else if (gameObjectType.compare("Tank_Scout") == 0) {
			g = new Tank_Scout(gameObjDataMap, this);
		}
		else if (gameObjectType.compare("Tank_Sniper") == 0) {
			g = new Tank_Sniper(gameObjDataMap, this);
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

	msgBus->postMessage(new Msg(GO_ADDED, oss.str()), this);

	//set the object's parent if there is one
	if (g->parentId != "") {
		for (GameObject* p : gameObjects) {
			if (p->id == g->parentId) {
				g->setParent(p);
			}
		}
	}

	//add to the collisions quad tree, if colliisons are enabled
	if (g->collisionsEnabled==1)
		insertIntoQuadTree(quadTreeRoot, g);
	
		
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

		//handle collisions using the quad tree
		handleCollisions();

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
		if (g->id == objectID) {
			if (TankObject* tank = dynamic_cast<TankObject*>(g)) {
				//OutputDebugString("\nFOund Tank\n");
				//TankObject* tank = dynamic_cast<TankObject*>(g);
				return tank;
			}
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

//Position of tank firing: _originX, _originY
//how many tiles the shot can go: length
//axis: 0=r 1=l 2=ur 3=dl 4=ul 5=dr
/*void GameSystem::dealLineDamage(int _originX, int _originY, int length, int axis, int damage) {
	vector<TankObject *> thingsHit; //list of things hit

	for (GameObject *go : gameSystem->gameObjects) { //look through all gameobjects
		if (go->getObjectType() == "TankObject") {
			TankObject* tank = (TankObject*)go;
			if (sameAxisShot(axis, _originX, _originY, tank->gridX, tank->gridY, length)) { //if on same axis and in range
				thingsHit.push_back(tank); //add things that are in firing range along the axis to the list
			}
		}
	}

	//Find the first thing hit from the list
	if (thingsHit.size() > 0) {
		TankObject* currClosestTank = nullptr;
		int dist = -1;
		for (TankObject* t : thingsHit) {
			OutputDebugString((t->id).c_str());
			OutputDebugString(" hit\n");
			if (dist < getGridDistance(_originX, _originY, t->gridX, t->gridY)) {
				dist = getGridDistance(_originX, _originY, t->gridX, t->gridY);
				currClosestTank = t;
			}
		}

		//deal damage to closest tank;
		if (currClosestTank != nullptr) {
			currClosestTank->health -= damage;
			if (currClosestTank->health <= 0)
				msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, currClosestTank->id + ",1,crater.png,"), gameSystem);
			//updatePlayerHealthBar(currClosestTank->id);//move this to tankObject
		}
	}
}*/

bool GameSystem::sameAxisShot(int axis, int x1, int y1, int x2, int y2, int length) {
	bool result = false;

	int aXCube = x1 - (y1 - (y1 & 1)) / 2;
	int aZCube = y1;
	int aYCube = -aXCube - aZCube;

	int bXCube = x2 - (y2 - (y2 & 1)) / 2;
	int bZCube = y2;
	int bYCube = -bXCube - bZCube;

	int axisBase[3] = { 0, 0, 0 };

	switch (axis) {
	case 0: //r (+1, -1, 0)
		axisBase[0] = 1;
		axisBase[1] = -1;
		axisBase[2] = 0;
		break;

	case 1: //l (-1, +1, 0)
		axisBase[0] = -1;
		axisBase[1] = 1;
		axisBase[2] = 0;
		break;

	case 2: //ur (0, -1, 1)
		axisBase[0] = 0;
		axisBase[1] = -1;
		axisBase[2] = 1;
		break;

	case 3: //dl (0, 1, -1)
		axisBase[0] = 0;
		axisBase[1] = 1;
		axisBase[2] = -1;
		break;

	case 4: //ul (-1, 0, 1)
		axisBase[0] = -1;
		axisBase[1] = 0;
		axisBase[2] = 1;
		break;

	case 5: //dr (1, 0, -1)
		axisBase[0] = 1;
		axisBase[1] = 0;
		axisBase[2] = -1;
		break;
	}

	//Test if the point line up on the specified axis
	for (int i = 1; i < length; i++) {
		int tempaXCube = aXCube + axisBase[0] * i;
		int tempaYCube = aYCube + axisBase[1] * i;
		int tempaZCube = aZCube + axisBase[2] * i;

		if ((tempaXCube == bXCube)
			&& (tempaYCube == bYCube)
			&& (tempaZCube == bZCube)) {
			result = true;
			break;
		}
	}

	return result;
}

//For selecting a direction to shoot along an axis
//position of player's tank: (x1, y1)
//position of click: (x2, y2)
//returns 0=r 1=l 2=ur 3=dl 4=ul 5=dr -1=none
int GameSystem::onAxis(int x1, int y1, int x2, int y2, int range) {
	if (x1 == x2 && y1 == y2) { //same location
		return -1;
	}

	int aXCube = x1 - (y1 - (y1 & 1)) / 2;
	int aZCube = y1;
	int aYCube = -aXCube - aZCube;

	int bXCube = x2 - (y2 - (y2 & 1)) / 2;
	int bZCube = y2;
	int bYCube = -bXCube - bZCube;

	//Check directions then return if it is in that direction

	//r (+1, -1, 0)
	int r[3] = { 1, -1, 0 };
	for (int i = 1; i < range; i++) {
		int tempaXCube = aXCube + r[0] * i;
		int tempaYCube = aYCube + r[1] * i;
		int tempaZCube = aZCube + r[2] * i;

		if ((tempaXCube == bXCube)
			&& (tempaYCube == bYCube)
			&& (tempaZCube == bZCube)) {
			OutputDebugString("right\n");
			return 0;
		}
	}

	//l (-1, +1, 0)
	int l[3] = { -1, 1, 0 };
	for (int i = 1; i < range; i++) {
		int tempaXCube = aXCube + l[0] * i;
		int tempaYCube = aYCube + l[1] * i;
		int tempaZCube = aZCube + l[2] * i;

		if ((tempaXCube == bXCube)
			&& (tempaYCube == bYCube)
			&& (tempaZCube == bZCube)) {
			OutputDebugString("left\n");
			return 1;
		}
	}

	//ur (0, -1, 1)
	int ur[3] = { 0, -1, 1 };
	for (int i = 1; i < range; i++) {
		int tempaXCube = aXCube + ur[0] * i;
		int tempaYCube = aYCube + ur[1] * i;
		int tempaZCube = aZCube + ur[2] * i;

		if ((tempaXCube == bXCube)
			&& (tempaYCube == bYCube)
			&& (tempaZCube == bZCube)) {
			OutputDebugString("up right\n");
			return 2;
		}
	}

	//dl (0, 1, -1)
	int dl[3] = { 0, 1, -1 };
	for (int i = 1; i < range; i++) {
		int tempaXCube = aXCube + dl[0] * i;
		int tempaYCube = aYCube + dl[1] * i;
		int tempaZCube = aZCube + dl[2] * i;

		if ((tempaXCube == bXCube)
			&& (tempaYCube == bYCube)
			&& (tempaZCube == bZCube)) {
			OutputDebugString("down left\n");
			return 3;
		}
	}

	//ul (-1, 0, 1)
	int ul[3] = { -1, 0, 1 };
	for (int i = 1; i < range; i++) {
		int tempaXCube = aXCube + ul[0] * i;
		int tempaYCube = aYCube + ul[1] * i;
		int tempaZCube = aZCube + ul[2] * i;

		if ((tempaXCube == bXCube)
			&& (tempaYCube == bYCube)
			&& (tempaZCube == bZCube)) {
			OutputDebugString("up left\n");
			return 4;
		}
	}

	//dr (1, 0, -1)
	int dr[3] = { 1, 0, -1 };
	for (int i = 1; i < range; i++) {
		int tempaXCube = aXCube + dr[0] * i;
		int tempaYCube = aYCube + dr[1] * i;
		int tempaZCube = aZCube + dr[2] * i;

		if ((tempaXCube == bXCube)
			&& (tempaYCube == bYCube)
			&& (tempaZCube == bZCube)) {
			OutputDebugString("down right\n");
			return 5;
		}
	}

	return -1; //Not on any axis or in range
}

//check if there is a collisiion between the two Gameobjects
//gameObject hitboxes are currently rectangular, with no rotation. 
bool GameSystem::checkCollision(GameObject* a, GameObject* b) {
	if ((a->x + a->width >= b->x - b->width)
		&& (a->x - a->width <= b->x + b->width)
		&& (a->y + a->length >= b->y - b->length)
		&& (a->y - a->length <= b->y + b->length)) {
		b->onCollide(a);
		return true;
	}
		
	return false;
}

//collision between a collision quadTree node and a gameobject
//gameObject hitboxes are currently rectangular, with no rotation. Nodes are always square, with no rotation
bool GameSystem::checkCollision(quadTreeNode* a, GameObject* b) {
	if ((a->x + a->size >= b->x - b->width)
		&& (a->x - a->size <= b->x + b->width)
		&& (a->y + a->size >= b->y - b->length)
		&& (a->y - a->size <= b->y + b->length))
		return true;
	return false;
}

//inset the gameObject into the collision quad tree
void GameSystem::insertIntoQuadTree(quadTreeNode* root, GameObject* g) {
	bool gInNode = checkCollision(root, g);
	if (gInNode) {
		root->isEmpty = false;
		root->containedObjects.push_back(g);

		//if there this is a leaf node check if it neads to be extended
		if (root->isLeaf && root->size > 8 && root->containedObjects.size()>1) {
			root->isLeaf = false;
			root->childNodes[0] = new quadTreeNode(root->x - root->size / 2, root->y + root->size / 2, root->size / 2);
			root->childNodes[1] = new quadTreeNode(root->x + root->size / 2, root->y + root->size / 2, root->size / 2);
			root->childNodes[2] = new quadTreeNode(root->x - root->size / 2, root->y - root->size / 2, root->size / 2);
			root->childNodes[3] = new quadTreeNode(root->x + root->size / 2, root->y - root->size / 2, root->size / 2);
		}
		if (!root->isLeaf)
			for (quadTreeNode* q : root->childNodes)
				insertIntoQuadTree(q, g);
	}
}

//remove the object from the collision quad tree
void GameSystem::removeFromQuadTree(quadTreeNode* root, GameObject* g) {
	for (GameObject* cg : root->containedObjects) {
		if (cg == g) {
			root->containedObjects.erase(remove(root->containedObjects.begin(), root->containedObjects.end(), g), root->containedObjects.end());
			if (!root->isLeaf) {
				for (quadTreeNode* c : root->childNodes)
					removeFromQuadTree(c, g);
			}
		}
	}
}

//check if the the GameObject is colliding with anything, using the quadTree 
//returns true if there has been any collisions
bool GameSystem::checkTreeCollision(quadTreeNode* root, GameObject* g) {
	bool hitNode = checkCollision(root, g);

	if (hitNode && !root->isEmpty && root->isLeaf &&root->containedObjects.size() > 1) {
		for (GameObject* cg : root->containedObjects) {
			if (cg != g && checkCollision(cg, g))
				return true;
		}
	}

	//if hit root node and not empty check children
	if (hitNode && !root->isEmpty && !root->isLeaf && root->containedObjects.size() > 1) {
		for (quadTreeNode* q : root->childNodes) {
			if (checkTreeCollision(q, g))
				return true;
		}
	}
	return false;
}

//clears the all objects from the collision quad tree
void GameSystem::clearQuadTree(quadTreeNode* root) {
	if (root == nullptr)
		return;

	for (quadTreeNode* q : root->childNodes) {
		clearQuadTree(q);
	}
	delete root;
	return;
}

void GameSystem::handleCollisions() {
	for (GameObject* g : gameObjects) {
		if (g->collisionsEnabled) {
			removeFromQuadTree(quadTreeRoot, g);
			insertIntoQuadTree(quadTreeRoot, g);

			checkTreeCollision(quadTreeRoot, g);
		}
	}
}