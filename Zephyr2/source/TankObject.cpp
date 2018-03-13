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