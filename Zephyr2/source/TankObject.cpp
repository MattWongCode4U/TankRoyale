#include "TankObject.h"

TankObject::TankObject(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : GridObject(paramsMap, _gameSystemUtil) {
	health = stoi(paramsMap.find("health")->second);
	maxHealth = health;
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

void TankObject::takeDamage(int damage) {
	OutputDebugString("\nBASE TakeDamgage\n");
	health -= damage;
	updatehpBar();
}

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
				if (tank->health <= 0)
					gameSystemUtil->postMessageToBus(new Msg(UPDATE_OBJ_SPRITE, tank->id + ",1,crater.png,"));
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
	hpBar->width*= ((float)health / maxHealth);

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

//checks if the targeted move action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int TankObject::checkMoveValidity(int originX, int originY, int targetX, int targetY) {
	if (gameSystemUtil->getGridDistance(originX, originY, targetX, targetY) == 1)
		return 1;
	else
		return -1;
}

//checks if the targeted Shooting action is valid for this tankObject
//returns -1 if action invalid
//returns the action cost if valid
int TankObject::checkShootValidity(int originX, int originY, int targetX, int targetY) {
	int distancetoTarget = gameSystemUtil->getGridDistance(originX, originY, targetX, targetY);

	if (distancetoTarget < 6 && distancetoTarget > 0)
		return 2;
	else
		return -1;
}

//params int turn amount & direction (6 or -6 is full 360. 
void TankObject::turn(int turnDir) {

}