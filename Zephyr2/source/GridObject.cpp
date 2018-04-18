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
		moveTowards(newX, newY, z, zRotation, frameDelay);
}

vect2 GridObject::gridToWorlPos(int gridX, int gridY) {
	float hexHeight = hexSize * 2.0f; //height of a single hex tile
	float vertDist = hexHeight * 3.0f / 4.0f;//verticle distance between tile center points
	float hexWidth = sqrt(3.0f) / 2.0f * hexHeight;//width of a single tile. Also the horizontal distance bewteen 2 tiles

	vect2 v;
	v.x= hexWidth * gridX;

	if (gridY % 2 != 0) {
		v.x += hexWidth / 2;
	}
	v.y = vertDist * (float)gridY;

	return v;
}

int GridObject::getAxisOrientation(GridObject* g) {
	//if no grid object specified, find orientation for this object
	if (g == 0)
		g = this;

	int degrees = ((int)g->zRotation) % 360;
	switch (degrees) {
	case 0: //r
		return 0;
		break;
	case 180: //l
		return 1;
		break;
	case 60: //ur
		return 2;
		break;
	case 240: //dl
		return 3;
		break;
	case 120: //ul
		return 4;
		break;
	case 300: //dr
		return 5;
		break;
	}
}