#include "Cannonball.h"

Cannonball::Cannonball(string _id, std::string renderableName, int xpos, int ypos, int _orientation, int _width, int _length, ObjectData* _objData) {
	objData = _objData;
	id = _id;
	renderable = renderableName;
	x = xpos;
	y = ypos;
	orientation = _orientation;
	width = _width;
	length = _length;
	physicsEnabled = 1;
}

Cannonball::Cannonball(map <string, string> paramsMap, ObjectData* objData) : GameObject(paramsMap, objData) {
	
}

string Cannonball::toString() {
	string output = GameObject::toString();
	output += "\nxpos: " + to_string(x) + ",";
	output += "\nypos: " + to_string(y) + ",";
	return output;
}

Cannonball::~Cannonball() {
}

string Cannonball::getObjectType() {
	return "Cannonball";
}

void Cannonball::earlyUpdate() {
}

void Cannonball::midUpdate() {
	counter++;

	if (counter > 120 && counter < 135) {
		std::ostringstream oss;
		oss << id << "," << "1" << "," << deathSprite;
		//Msg* m = new Msg(UPDATE_OBJ_SPRITE, oss.str());
		objData->toPostVector.push_back(new Msg(UPDATE_OBJ_SPRITE, oss.str()));
		//objData->toPostVector.push_back(new Msg(CHANGE_MAST, "shipwreck,0,Boat_S2.png"));	
	}

	if (counter > 135) {
		objData->toDestroyVector.push_back(this);
	}

}
void Cannonball::lateUpdate() {

}

void Cannonball::onCollide(GameObject* otherObj) {
	if (counter > 21 && otherObj != parentObject && otherObj->getObjectType() != "Cannonball") {
		objData->toDestroyVector.push_back(this);

		//deathSprite = "explosion.png";
		counter = 100;

		//send message to alert AI that it scored a hit
		if (parentObject->id.find("enemy") != string::npos || id.find("enemy") != string::npos) {
			ostringstream oss;
			oss << parentObject->id << "," << otherObj->id << "," << id;
			objData->toPostVector.push_back(new Msg(SCORED_HIT, oss.str()));
		}
	}
}
