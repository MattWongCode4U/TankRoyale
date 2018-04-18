#include "GameObject.h"

class Projectile : public GameObject {
public:
	Projectile(map<string, string> params, GameSystemUtil* _gameSystemUtil);

	//returns the object type string e.g. "GridObject"
	std::string getObjectType();

	void onCollide(GameObject* otherObj) override;

	void midUpdate();

	//calculate movement offsets and apply them each turn;
	void calculateVelocity();
	
	//the tank that shot the projectile. 
	GameObject* firingTank;

	//how much the projectile's position in offset each frame;
	float velocityX = 0;
	float velocityY = 0;

	//how fast the projectile should be moving
	float speed;

	//target position
	float targetX;
	float targetY;

	//number of frames the projectile lives
	int lifeTime = 100;

	//distance object flies before being destroyed
	float range;

	//how many frames the projectile has been alive for
	int age = 0;

};