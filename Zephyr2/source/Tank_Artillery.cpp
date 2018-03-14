#include "Tank_Artillery.h"

Tank_Artillery::Tank_Artillery(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : TankObject(paramsMap, _gameSystemUtil) {
}

Tank_Artillery::~Tank_Artillery() {
}

string Tank_Artillery::getObjectType() {
	return "Tank_Artillery";
}

void Tank_Artillery::takeDamage(int damage) {
	OutputDebugString("\nARTI Damaged\n");
	health -= damage;
	updatehpBar();
}

void Tank_Artillery::shoot(int targetX, int targetY) {
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
int Tank_Artillery::checkMoveValidity(int originX, int originY, int targetX, int targetY) {
	if (gameSystemUtil->getGridDistance(originX, originY, targetX, targetY) == 1)
		return 1;
	else
		return -1;
}

//checks if the targeted Shooting action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int Tank_Artillery::checkShootValidity(int originX, int originY, int targetX, int targetY) {
	int range = 6;
	int distancetoTarget = gameSystemUtil->getGridDistance(originX, originY, targetX, targetY);

	if (distancetoTarget < range && distancetoTarget > 0)
		return 2;
	else
		return -1;
}