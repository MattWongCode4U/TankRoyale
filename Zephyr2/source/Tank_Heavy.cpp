#include "Tank_Heavy.h"

Tank_Heavy::Tank_Heavy(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : TankObject(paramsMap, _gameSystemUtil) {
	shootOverlayRenderable = "range_Shoot_Heavy.png";
	moveOverlayRenderable = "range_Move.png";
	shootOverlaySize = 40;
	moveOverlaySize = 20;
}

Tank_Heavy::~Tank_Heavy() {
}

void Tank_Heavy::takeDamage(int damage) {
	OutputDebugString("\nHEAVY Damaged\n");
	health -= damage;
	updatehpBar();
}

string Tank_Heavy::getObjectType() {	
	return "Tank_Heavy";
}

void Tank_Heavy::shoot(int targetX, int targetY) {
	OutputDebugString("\nHEAVY SHOT\n");
	int range = 4;
	int damage = 29;
	int aXCube = targetX - (targetY - (targetY & 1)) / 2;
	int aZCube = targetY;
	int aYCube = -aXCube - aZCube;
	vector<GameObject*>* gameObjects = gameSystemUtil->getGameObjectsVector();


	//create explosion Object
	GridObject* go = (GridObject*)gameSystemUtil->makeGameObject("explostion.txt");
	go->id = "explosion" + to_string(rand());
	go->gridX = targetX;
	go->gridY = targetY;
	gameSystemUtil->createGameObject(go);
	go->updateWorldCoords();

	vector<TankObject *> thingsHit; //list of things hit
	int axis = gameSystemUtil->onAxis(gridX, gridY, targetX, targetY, range);
	for (GameObject *go : *gameObjects) { //look through all gameobjects
		if (TankObject* tank = dynamic_cast<TankObject*>(go)) {
			if (gameSystemUtil->sameAxisShot(axis, gridX, gridY, targetX, targetY, range)) {//if on same axis and in range
				thingsHit.push_back(tank);//add things that are in firing range along the axis to the list
			}
		}
	}

	//Find the first thing hit from the list
	if (thingsHit.size() > 0) {
		TankObject* currClosestTank = nullptr;
		int dist = INT32_MAX;
		for (TankObject* t : thingsHit) {
			OutputDebugString((t->id).c_str());
			OutputDebugString(" hit\n");
			if (dist > gameSystemUtil->getGridDistance(gridX, gridY, t->gridX, t->gridY)) {
				dist = gameSystemUtil->getGridDistance(gridX, gridY, t->gridX, t->gridY);
				currClosestTank = t;
			}
		}

		//deal damage to closest tank;
		if (currClosestTank != nullptr) {
			currClosestTank->takeDamage(damage);
			if (currClosestTank->health <= 0)
				gameSystemUtil->postMessageToBus(new Msg(UPDATE_OBJ_SPRITE, currClosestTank->id + ",1,crater.png,"));
		}
	}
}


//checks if the targeted move action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int Tank_Heavy::checkMoveValidity(GridObject* originObject, int targetX, int targetY) {
	int range = 2;
	int axis = gameSystemUtil->onAxis(originObject->gridX, originObject->gridY, targetX, targetY, range);

	if (axis != -1 && axis == getAxisOrientation(originObject))
		return 1;
	else
		return -1;
}

//checks if the targeted Shooting action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int Tank_Heavy::checkShootValidity(GridObject* originObject, int targetX, int targetY) {
	int range = 4;
	int axis = gameSystemUtil->onAxis(originObject->gridX, originObject->gridY, targetX, targetY, range);

	if (axis != -1)
		return 1;
	else
		return -1;
}