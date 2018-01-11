#include "FullscreenObj.h"

FullscreenObj::FullscreenObj(int idNum, std::string renderableName, int xpos, int ypos, int orientation) {
	id = idNum;
	renderable = renderableName;
	x = xpos;
	y = ypos;
	orientation = orientation;
	direction = 0;
}

FullscreenObj::FullscreenObj(map <string, string> paramsMap, ObjectData* objData) : GameObject(paramsMap, objData) {
	
}

string FullscreenObj::toString() {
	string output = GameObject::toString();
	output += "\nsail: " + to_string(sail) + ",";
	output += "\nrudder: " + to_string(rudder) + ",";
	return output;
}

FullscreenObj::~FullscreenObj() {
}

string FullscreenObj::getObjectType() {
	return "FullscreenObj";
}


void FullscreenObj::earlyUpdate() {

}
void FullscreenObj::midUpdate() {

}
void FullscreenObj::lateUpdate() {

}