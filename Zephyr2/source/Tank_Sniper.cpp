#include "Tank_Sniper.h"

Tank_Sniper::Tank_Sniper(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : TankObject(paramsMap, _gameSystemUtil) {
}

Tank_Sniper::~Tank_Sniper() {
}

void Tank_Sniper::takeDamage(int damage) {
	health -= damage;
	updatehpBar();
}

void Tank_Sniper::shoot(int targetX, int targetY) {
	int range = 5;
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
		int dist = -1;
		for (TankObject* t : thingsHit) {
			OutputDebugString((t->id).c_str());
			OutputDebugString(" hit\n");
			if (dist < gameSystemUtil->getGridDistance(gridX, gridY, t->gridX, t->gridY)) {
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
int Tank_Sniper::checkMoveValidity(GridObject* originObject, int targetX, int targetY) {
	int range = 2;
	int axis = gameSystemUtil->onAxis(originObject->gridX, originObject->gridY, targetX, targetY, range);

	if (axis != -1 && axis == getAxisOrientation(originObject))
		return 1;
	else
		return -1;
}

string Tank_Sniper::getObjectType() {
	return "Tank_Sniper";
}

//checks if the targeted Shooting action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int Tank_Sniper::checkShootValidity(GridObject* originObject, int targetX, int targetY) {
	int range = 7;
	int axis = gameSystemUtil->onAxis(originObject->gridX, originObject->gridY, targetX, targetY, range);
	
	if (axis != -1 && axis == getAxisOrientation(originObject))
		return 1;
	else
		return -1;
}