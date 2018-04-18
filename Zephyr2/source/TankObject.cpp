#include "TankObject.h"

TankObject::TankObject(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : GridObject(paramsMap, _gameSystemUtil) {
	health = stoi(paramsMap.find("health")->second);
	maxHealth = health;
}

//returns the string representation of the objects data. Compatible with save file syntax
string TankObject::toString() {
	string output = GameObject::toString();
	output += "\nhealth: " + to_string(health) + ",";
	return output;
}

TankObject::~TankObject() {
}

//returns the object type string e.g. "GridObject"
string TankObject::getObjectType() {
	return "TankObject";
}
//returns the tanks health
int TankObject::getHealth() {
	// this->id
	return this->health;
}
//sets the tanks health to the specified value
void TankObject::setHealth(int newHealth) {
	this->health = newHealth;
}
//updates the tanks health minus the damage taken
void TankObject::takeDamage(int damage) {
	OutputDebugString("\nBASE TakeDamgage\n");
	health -= damage;
	updatehpBar();
}
//Creates projectile and shoots at the coordinates
void TankObject::shoot(int targetX, int targetY) {
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
				if (tank->health <= 0) {
					gameSystemUtil->postMessageToBus(new Msg(UPDATE_OBJ_SPRITE, tank->id + ",1,crater.png,"));
					tank->renderable = "crater.png";
					tank->collisionsEnabled = false;
				}
					
			}
		}
	}
}

//creates an hp bar object for this tank
void TankObject::createhpBar() {
	//SET HP BAR
	hpBar = gameSystemUtil->makeGameObject("hpBar.txt");

	//set initial values
	hpBar->id = id + "_hpBar";
	hpBar->parentId = id;

	//add the new Gameobject to The game
	gameSystemUtil->createGameObject(hpBar);

	hpBarWidth = hpBar->width;//set the original width of the hp bar
}

//updates the hpBar
void TankObject::updatehpBar() {
	//set bar width
	if (hpBar != nullptr) {
		hpBar->width *= ((float)health / maxHealth);

		//set color
		if ((float)health / maxHealth < .3f)
			hpBar->renderable = "red_hpbar";
		else if ((float)health / maxHealth < .5f)
			hpBar->renderable = "orange_hpbar";
		else if ((float)health / maxHealth > .8f)
			hpBar->renderable = "green_hpbar";

		//post updated information message for the renderer
		hpBar->postPostionMsg();
	}
}

//checks if the targeted move action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int TankObject::checkMoveValidity(GridObject* originObject, int targetX, int targetY) {
	if (gameSystemUtil->getGridDistance(originObject->gridX, originObject->gridY, targetX, targetY) == 1)
		return 1;
	else
		return -1;
}

//checks if the targeted Shooting action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int TankObject::checkShootValidity(GridObject* originObject, int targetX, int targetY) {

	int distancetoTarget = gameSystemUtil->getGridDistance(originObject->gridX, originObject->gridY, targetX, targetY);

	if (distancetoTarget < 6 && distancetoTarget > 0)
		return 2;
	else
		return -1;
}

//checks if the turn is a valid move
int TankObject::checkTurnValidity(int originX, int originY, int targetX, int targetY) {
	if (gameSystemUtil->getGridDistance(originX, originY, targetX, targetY) == 0)
		return 1;
	else
		return -1;
}