#include "TankObject.h"

TankObject::TankObject(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : GridObject(paramsMap, _gameSystemUtil) {
	health = stoi(paramsMap.find("health")->second);
}

string TankObject::toString() {
	string output = GameObject::toString();
	output += "\nhealth: " + to_string(health) + ",";
	return output;
}

TankObject::~TankObject() {
}

string TankObject::getObjectType() {
	return "TankObject";
}

int TankObject::getHealth() {
	// this->id
	return this->health;
}

void TankObject::setHealth(int newHealth) {
	this->health = newHealth;
}

void TankObject::shoot(int targetX, int targetY) {
	int affectedRadius = 1;
	int damage = 19;
	int aXCube = targetX - (targetY - (targetY & 1)) / 2;
	int aZCube = targetY;
	int aYCube = -aXCube - aZCube;
	vector<GameObject*>* gameObjects = gameSystemUtil->getGameObjectsVector();

	for (GameObject *go : *gameObjects) { //look through all gameobjects
		if (go->getObjectType() == "TankObject") {
			TankObject* tank = (TankObject*)go;
			if (gameSystemUtil->getGridDistance(targetX, targetY, tank->gridX, tank->gridY) <= affectedRadius) {
				tank->health -= damage;
				if (tank->health <= 0)
					gameSystemUtil->postMessageToBus(new Msg(UPDATE_OBJ_SPRITE, tank->id + ",1,crater.png,"));
				//updatePlayerHealthBar(tank->id);
				OutputDebugString(tank->id.c_str());
				OutputDebugString(" TOOK DAMAGE\n");
			}
		}
	}
}
