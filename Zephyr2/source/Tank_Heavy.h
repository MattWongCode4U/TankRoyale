#pragma once
#include "TankObject.h"

//const int TANK_MAX_HEALTH = 100;

//Heavy tank class
class Tank_Heavy : public TankObject {
public:
	Tank_Heavy(map<string, string> params, GameSystemUtil* _gameSystemUtil);
	~Tank_Heavy();

	//returns the object type string e.g. "GridObject"
	std::string getObjectType();

	//Updates the tank's health with damage dealt
	void takeDamage(int damage);

	//Creates projectile and shoots at the coordinates
	void shoot(int targetX,int targetY) override;

	//checks if the targeted move action is valid for this tankObject
	//returns -1 if action invalid
	//returns the action cost if valid
	int checkMoveValidity(GridObject* originObject, int targetX, int targetY) override;

	//checks if the targeted Shooting action is valid for this tankObject
	//returns -1 if action invalid
	//returns the action cost if valid
	int checkShootValidity(GridObject* originObject, int targetX, int targetY) override;

	
};