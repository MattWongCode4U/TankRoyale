#include "projectile.h"

Projectile::Projectile(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : GameObject(paramsMap, _gameSystemUtil) {
	//gridX = stoi(paramsMap.find("gridX")->second);
	//gridY = stoi(paramsMap.find("gridY")->second);
}

string Projectile::getObjectType() {
	return "Projectile";
}

//called by the checkColliision() in gameSystem. 
//should get called every frame that the object is colliding with another object 
//only used by objects with the collisions enabled flag set to true
void Projectile::onCollide(GameObject* otherObj) {
	if (otherObj == firingTank) {
		return;
	//	OutputDebugString("Projectile HIT OWN TANK\n");
	}
	
	OutputDebugString("\nProjectile HIT ");
	OutputDebugString(otherObj->id.c_str());


	//create explosion Object
	GameObject* go = gameSystemUtil->makeGameObject("explostion.txt");
	go->id = "explosion" + to_string(rand());
	go->x = x;
	go->y = y;
	go->z = z;
	go->width = 5;
	go->length = 5;
	go->animationDelay = 2;
	gameSystemUtil->createGameObject(go);

	gameSystemUtil->deleteGameObject(this);
}

void Projectile::midUpdate() {
	offsetPosition(velocityX, velocityY, 0, 0);

	age++;
	if(age >lifeTime)
		gameSystemUtil->deleteGameObject(this);
}

void Projectile::calculateVelocity() {
		
	float mag = sqrt(pow(targetX - x, 2) + pow(targetY - y, 2)); //calculate magnitude

	//normalize the direction vector
	velocityX = (targetX - x) / mag * speed;
	velocityY = (targetY - y) / mag * speed;

	lifeTime = (int)range / speed;

	//set bullet to face firing direction
	//zRotation = atan((x- targetX) / (y - targetY)) * 180 /3.1415f;


}