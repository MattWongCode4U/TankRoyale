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

	for (GameObject *go : *gameObjects) { //look through all gameobjects
		if (go->getObjectType() == "TankObject") {
			TankObject* tank = (TankObject*)go;
			if (gameSystemUtil->getGridDistance(targetX, targetY, tank->gridX, tank->gridY) <= affectedRadius) {
				tank->takeDamage(damage);
				if (tank->health <= 0)
					gameSystemUtil->postMessageToBus(new Msg(UPDATE_OBJ_SPRITE, tank->id + ",1,crater.png,"));
				//updatePlayerHealthBar(tank->id);
				OutputDebugString(tank->id.c_str());
				OutputDebugString(" TOOK DAMAGE\n");
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