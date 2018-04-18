#pragma once
#include "GridObject.h"
#include "Projectile.h"

const int TANK_MAX_HEALTH = 100;

//Base tank object that all tank classes derrive from
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


	//Heatlh value
	int health = 100;
	//Health functions
	int getHealth();
	void setHealth(int newHealth);
	virtual void takeDamage(int damage);//reduce health and update hpBar

	//Health bar
	GameObject* hpBar;//the id of this tank hp bar
	void createhpBar();//creates an hp bar object for this tank
	void updatehpBar();//updates the hpBar

	//checks if the targeted move action is valid for this tankObject
	//returns -1 if action invalid
	//returns the action cost if valid
	virtual int checkMoveValidity(GridObject* originObject, int targetX, int targetY);

	//checks if the targeted Shooting action is valid for this tankObject
	//returns -1 if action invalid
	//returns the action cost if valid
	//virtual int checkShootValidity(int originX, int originY, int targetX, int targetY);
	virtual int checkShootValidity(GridObject* originObject, int targetX, int targetY);
	
	//checks if the turn is a valid move
	int checkTurnValidity(int originX, int originY, int targetX, int targetY);

	//the width of a full hpBar
	float hpBarWidth;//the original width of the hp bar
	float maxHealth;//the maximum hp pool

	//projectile values
	string shootOverlayRenderable = "range_Shoot_Artillery.png";
	string moveOverlayRenderable = "range_Move.png";
	float shootOverlaySize = 60;
	float moveOverlaySize = 20;

	// Check if still in game
	bool outOfMatch = false;

	//int getAxisOrientation();

};