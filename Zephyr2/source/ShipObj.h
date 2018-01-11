#pragma once
#include "GameObject.h"
#include "Cannonball.h"

class ShipObj :
	public GameObject {
public:
	ShipObj(int idNum, std::string renderableName, int xpos, int ypos, int orientation);
	//DummyGameObj(std::vector<std::string> params);
	ShipObj(map<string, string> params, ObjectData* objData);
	~ShipObj();
	virtual std::string getObjectType();
	virtual std::string toString();
	void earlyUpdate();
	void midUpdate();
	void lateUpdate();
	void onCollide(GameObject* otherObj) override;
	void shoot(std::string direction);
	int sail;//the sail position 0-2
	int rudder;// 0-4 rudder position
	int health = 100;

	int counter = 0;
	int reloadCounter = 0;
};

