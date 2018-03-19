#pragma once
#include "GridObject.h"

const int TANK_MAX_HEALTH = 100;

class TankObject : public GridObject {
public:
	TankObject(map<string, string> params, GameSystemUtil* _gameSystemUtil);
	~TankObject();

	//returns the object type string e.g. "GridObject"
	std::string getObjectType();

	//returns the string representation of the objects data. Compatible with save file syntax
	std::string toString();

	//deals damage to affected objects. creates animations
	virtual void shoot(int targetX,int targetY);


	//the remaining hitpoints
	int health = 100;
	int getHealth();
	void setHealth(int newHealth);
	virtual void takeDamage(int damage);//reduce health and update hpBar

	GameObject* hpBar;//the id of this tank hp bar
	void createhpBar();//creates an hp bar object for this tank
	void updatehpBar();//updates the hpBar

	//checks if the targeted move action is valid for this tankObject
	//returns -1 if action invalid
	//returns the action cost if valid
	virtual int checkMoveValidity(int originX, int originY, int targetX, int targetY);

	//checks if the targeted Shooting action is valid for this tankObject
	//returns -1 if action invalid
	//returns the action cost if valid
	virtual int checkShootValidity(int originX, int originY, int targetX, int targetY);

	int checkTurnValidity(int originX, int originY, int targetX, int targetY);

	//the width of a full hpBar
	float hpBarWidth;//the original width of the hp bar
	float maxHealth;//the maximum hp pool
	
	void turn(int turnDir);
	int getAxisOrientation();
};