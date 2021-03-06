#include "Tank_Scout.h"

Tank_Scout::Tank_Scout(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : TankObject(paramsMap, _gameSystemUtil) {
	shootOverlayRenderable = "range_Shoot_Heavy.png";
	moveOverlayRenderable = "range_MoveLong.png";
	shootOverlaySize = 30;
	moveOverlaySize = 30;
}

Tank_Scout::~Tank_Scout() {
}
//Updates the tank's health with damage dealt
void Tank_Scout::takeDamage(int damage) {
	OutputDebugString("\nSCOUT Damaged\n");
	health -= damage;
	updatehpBar();
}

//Creates projectile and shoots at the coordinates
void Tank_Scout::shoot(int targetX, int targetY) {
	int range = 3;
	int damage = 29;
	int aXCube = targetX - (targetY - (targetY & 1)) / 2;
	int aZCube = targetY;
	int aYCube = -aXCube - aZCube;
	vector<GameObject*>* gameObjects = gameSystemUtil->getGameObjectsVector();

	//create projectile Object
	Projectile* p = (Projectile*)gameSystemUtil->makeGameObject("projectile.txt");
	p->id = "projectile" + to_string(rand());
	p->x = x;
	p->y = y;

	p->firingTank = this;

	vect2 v = gridToWorlPos(targetX, targetY);
	p->targetX = v.x;
	p->targetY = v.y;
	p->speed = 1;
	p->range = 20;
	p->calculateVelocity();
	gameSystemUtil->createGameObject(p);

	vector<TankObject *> thingsHit; //list of things hit
	int axis = gameSystemUtil->onAxis(gridX, gridY, targetX, targetY, range);
	for (GameObject *go : *gameObjects) { //look through all gameobjects
		if (TankObject* tank = dynamic_cast<TankObject*>(go)) {
			if (gameSystemUtil->sameAxisShot(axis, gridX, gridY, tank->gridX, tank->gridY, range)) {//if on same axis and in range
				thingsHit.push_back(tank);//add things that are in firing range along the axis to the list
			}
		}
	}

	//Find the first thing hit from the list
	if (thingsHit.size() > 0) {
		TankObject* currClosestTank = nullptr;
		int dist = INT32_MAX;
		for (TankObject* t : thingsHit) {
			OutputDebugString((t->id).c_str());
			OutputDebugString(" hit\n");
			int curDist = gameSystemUtil->getGridDistance(gridX, gridY, t->gridX, t->gridY);
			if (dist > curDist && curDist > 0) {
				if (t->health > 0) {
					dist = curDist;
					currClosestTank = t;
				}
			}
		}

		//deal damage to closest tank;
		if (currClosestTank != nullptr) {
			currClosestTank->takeDamage(damage);
			if (currClosestTank->health <= 0) {
				gameSystemUtil->postMessageToBus(new Msg(UPDATE_OBJ_SPRITE, currClosestTank->id + ",1,crater.png,"));
				currClosestTank->collisionsEnabled = false;
				currClosestTank->renderable = "crater.png";
			}
				
		}
	}
}

//checks if the targeted move action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int Tank_Scout::checkMoveValidity(GridObject* originObject, int targetX, int targetY) {
	int range = 3;
	int axis = gameSystemUtil->onAxis(originObject->gridX, originObject->gridY, targetX, targetY, range);

	if (axis != -1 && axis == getAxisOrientation(originObject))
		return 1;
	else
		return -1;
}

//Returns the objects type as a string
string Tank_Scout::getObjectType() {
	return "Tank_Scout";
}

//checks if the targeted Shooting action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int Tank_Scout::checkShootValidity(GridObject* originObject, int targetX, int targetY) {
	int range = 3;
	int axis = gameSystemUtil->onAxis(originObject->gridX, originObject->gridY, targetX, targetY, range);

	if (axis != -1)
		return 1;
	else
		return -1;
}