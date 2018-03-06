#include "GridObject.h"

GridObject::GridObject(map <string, string> paramsMap, ObjectData* objData) : GameObject(paramsMap, objData) {
	gridX = stoi(paramsMap.find("gridX")->second);
	gridY = stoi(paramsMap.find("gridY")->second);
}

GridObject::GridObject(ObjectData* _objData, std::string _id, std::string _renderable, float _x, float _y, int _z, int _orientation, int _width, int _length, int _imageFrames, int _gridX, int _gridY, string _parent) {
	objData = _objData;
	id = _id;
	renderable = _renderable;
	x = _x;
	y = _y;
	z = _z;
	zRotation = _orientation;
	width = _width;
	length = _length;
	height = 1.0f;
	imageFrames = _imageFrames;
	gridX = _gridX;
	gridY = _gridY;
	parentId = _parent;

	//set New renderable variables to default
	renderType = RenderableType::OVERLAY;
	model = "cube";
	normalMap = std::string();
	smoothness = 0.5f;
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
void GridObject::updateWorldCoords() {
	float hexHeight = hexSize * 2.0f; //height of a single hex tile
	float vertDist = hexHeight * 3.0f / 4.0f;//verticle distance between tile center points
	float hexWidth = sqrt(3.0f) / 2.0f * hexHeight;//width of a single tile. Also the horizontal distance bewteen 2 tiles
	
	float newX = hexWidth * gridX;

	if (gridY % 2 != 0) {
		newX += hexWidth / 2;
	}
	setPosition(newX, vertDist * gridY, z);
	//x = newX;
	//y = vertDist * gridY;
	//z = z;
}