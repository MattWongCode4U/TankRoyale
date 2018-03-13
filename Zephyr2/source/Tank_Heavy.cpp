#include "Tank_Heavy.h"

Tank_Heavy::Tank_Heavy(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : TankObject(paramsMap, _gameSystemUtil) {
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
int Tank_Heavy::checkMoveValidity(int originX, int originY, int targetX, int targetY) {
	if (gameSystemUtil->getGridDistance(originX, originY, targetX, targetY) == 1)
		return 1;
	else
		return -1;
}

//checks if the targeted Shooting action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int Tank_Heavy::checkShootValidity(int originX, int originY, int targetX, int targetY) {
	int distancetoTarget = gameSystemUtil->getGridDistance(originX, originY, targetX, targetY);

	if (distancetoTarget < 6 && distancetoTarget > 0)
		return 2;
	else
		return -1;
}