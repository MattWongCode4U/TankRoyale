#include "GridObject.h"

GridObject::GridObject(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) : GameObject(paramsMap, _gameSystemUtil) {
	gridX = stoi(paramsMap.find("gridX")->second);
	gridY = stoi(paramsMap.find("gridY")->second);
}

string GridObject::toString() {
	string output = GameObject::toString();
	output += "\ngridX: " + to_string(gridX) + ",";
	output += "\ngridY: " + to_string(gridY) + ",";
	return output;
}

GridObject::~GridObject() {
}

string GridObject::getObjectType() {
	return "GridObject";
}

void GridObject::earlyUpdate() {
}

void GridObject::midUpdate() {


}
void GridObject::lateUpdate() {

}

void GridObject::setGridCoords(int _gridX, int _gridY) {
	gridX = _gridX;
	gridY = _gridY;
	updateWorldCoords();
}

//sets world coords to match grid coordinates
void GridObject::updateWorldCoords(int frameDelay) {
	float hexHeight = hexSize * 2.0f; //height of a single hex tile
	float vertDist = hexHeight * 3.0f / 4.0f;//verticle distance between tile center points
	float hexWidth = sqrt(3.0f) / 2.0f * hexHeight;//width of a single tile. Also the horizontal distance bewteen 2 tiles
	
	float newX = hexWidth * gridX;

	if (gridY % 2 != 0) {
		newX += hexWidth / 2;
	}
	float newY = vertDist * (float)gridY;

	if(frameDelay == 0)
		setPosition(newX, vertDist * gridY, z);
	else
		moveTowards(newX, newY, z, 0, frameDelay);
	//x = newX;
	//y = vertDist * gridY;
	//z = z;
}