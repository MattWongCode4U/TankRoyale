#include "FullscreenObj.h"

FullscreenObj::FullscreenObj() {}; // Empty Constructor necessary

FullscreenObj::FullscreenObj(int idNum, std::string renderableName, float xpos, float ypos, int orientation, GameSystemUtil* _gameSystemUtil) {
	gameSystemUtil = _gameSystemUtil;
	id = idNum;
	renderable = renderableName;
	x = xpos;
	y = ypos;
	orientation = orientation;
	direction = 0;
}

FullscreenObj::FullscreenObj(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : GameObject(paramsMap, _gameSystemUtil) {
	
}

string FullscreenObj::toString() {
	string output = GameObject::toString();
	//output += "\nsail: " + to_string(sail) + ",";
	//output += "\nrudder: " + to_string(rudder) + ",";
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