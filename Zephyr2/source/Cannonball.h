#pragma once
#include "GameObject.h"


class Cannonball :
	public GameObject {
public:
	Cannonball(string _id, std::string renderableName, int xpos, int ypos, int _orientation, int _width, int _height, ObjectData* _objData);
	Cannonball(map<string, string> params, ObjectData* objData);
	~Cannonball();

	std::string getObjectType() override ;
	virtual std::string toString();
	void earlyUpdate();
	void midUpdate() override;
	void lateUpdate();
	void onCollide(GameObject* otherObj) override;
	std::string deathSprite = "WaterSplash.png";
	int counter = 0;
};

