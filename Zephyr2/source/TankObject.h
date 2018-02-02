#pragma once
#include "GridObject.h"

class TankObject : public GridObject {
public:
	TankObject(map<string, string> params, ObjectData* objData);
	~TankObject();

	//returns the object type string e.g. "GridObject"
	std::string getObjectType();

	//returns the string representation of the objects data. Compatible with save file syntax
	std::string toString();

	//the remaining hitpoints
	int health;
};