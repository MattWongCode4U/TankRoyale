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

	void shoot(int targetX,int targetY);


	//the remaining hitpoints
	int health = 100;
	int getHealth();
	void setHealth(int newHealth);
	void takeDamage(int damage);//reduce health and update hpBar

	GameObject* hpBar;//the id of this tank hp bar
	void createhpBar();//creates an hp bar object for this tank
	void updatehpBar();//updates the hpBar

	//the width of a full hpBar
	float hpBarWidth;//the original width of the hp bar
	float maxHealth;//the maximum hp pool
	
};