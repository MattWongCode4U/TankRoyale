#pragma once
#include "TankObject.h"

//const int TANK_MAX_HEALTH = 100;

class Tank_Artillery : public TankObject {
public:
	Tank_Artillery(map<string, string> params, GameSystemUtil* _gameSystemUtil);
	~Tank_Artillery();


	//returns the object type string e.g. "GridObject"
	std::string getObjectType();

	void takeDamage(int damage);//reduce health and update hpBar

	//deals damage and creates animations for the shooting action
	void shoot(int targetX,int targetY) override;

	//checks if the targeted move action is valid for this tankObject
	//returns -1 if action invalid
	//returns the action cost if valid
	int checkMoveValidity(int originX, int originY, int targetX, int targetY) override;

	//checks if the targeted Shooting action is valid for this tankObject
	//returns -1 if action invalid
	//returns the action cost if valid
	int checkShootValidity(int originX, int originY, int targetX, int targetY) override;

	
};