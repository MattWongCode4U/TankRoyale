#pragma once
#include "GameObject.h"


class Cannonball :
	public GameObject {
public:
	Cannonball(string _id, std::string renderableName, int xpos, int ypos, int _orientation, int _width, int _height, ObjectData* _objData);
	Cannonball(map<string, string> params, ObjectData* objData);
	~Cannonball();

	virtual std::string getObjectType();
	virtual std::string toString();
	void Cannonball::earlyUpdate();
	void Cannonball::midUpdate() override;
	void Cannonball::lateUpdate();
	void onCollide(GameObject* otherObj) override;
	std::string deathSprite = "WaterSplash.png";
	int counter = 0;
};

