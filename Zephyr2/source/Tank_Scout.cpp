#include "Tank_Scout.h"

Tank_Scout::Tank_Scout(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : TankObject(paramsMap, _gameSystemUtil) {
}

Tank_Scout::~Tank_Scout() {
}

void Tank_Scout::takeDamage(int damage) {
	OutputDebugString("\nSCOUT Damaged\n");
	health -= damage;
	updatehpBar();
}

void Tank_Scout::shoot(int targetX, int targetY) {
	int affectedRadius = 1;
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

	for (GameObject *go : *gameObjects) { //look through all gameobjects
		if (TankObject* tank = dynamic_cast<TankObject*>(go)) {
			if (gameSystemUtil->getGridDistance(targetX, targetY, tank->gridX, tank->gridY) <= affectedRadius) {
				tank->takeDamage(damage);
				if (tank->health <= 0)
					gameSystemUtil->postMessageToBus(new Msg(UPDATE_OBJ_SPRITE, tank->id + ",1,crater.png,"));
			}
		}
	}
}

//checks if the targeted move action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int Tank_Scout::checkMoveValidity(int originX, int originY, int targetX, int targetY) {
	if (gameSystemUtil->getGridDistance(originX, originY, targetX, targetY) == 1)
		return 1;
	else
		return -1;
}

string Tank_Scout::getObjectType() {
	return "Tank_Scout";
}

//checks if the targeted Shooting action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int Tank_Scout::checkShootValidity(int originX, int originY, int targetX, int targetY) {
	int distancetoTarget = gameSystemUtil->getGridDistance(originX, originY, targetX, targetY);

	if (distancetoTarget < 6 && distancetoTarget > 0)
		return 2;
	else
		return -1;
}