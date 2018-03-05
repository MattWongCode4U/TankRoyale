#include "GameObject.h"
using namespace std;
GameObject::GameObject() {
}

GameObject::~GameObject() {
}
GameObject::GameObject(map <string, string> paramsMap, ObjectData* _objData) {
	objData = _objData;
	try {
		id = paramsMap.find("id")->second;
		renderable = paramsMap.find("renderable")->second;
		
		x = stof(paramsMap.find("xPos")->second);
		y = stof(paramsMap.find("yPos")->second);
		z = stof(paramsMap.find("zPos")->second);
		orientation = stoi(paramsMap.find("orientation")->second);
		width = stoi(paramsMap.find("width")->second);
		length = stoi(paramsMap.find("length")->second);
		originalWidth = width;
		originalLength = length;
		if (!(paramsMap.find("parentId") == paramsMap.end()))
			parentId = paramsMap.find("parentId")->second;

		imageFrames = stoi(paramsMap.find("imageFrames")->second);
		if (!(paramsMap.find("renderType") == paramsMap.end()))
			renderType = getRenderableTypeFromName(paramsMap.find("renderType")->second);
		else
			renderType = RenderableType::OVERLAY;
		if (!(paramsMap.find("model") == paramsMap.end()))
			model = paramsMap.find("model")->second;
		else
			model = "cube";
		if (!(paramsMap.find("normalMap") == paramsMap.end()))
			normalMap = paramsMap.find("normalMap")->second;
		else
			normalMap = std::string();
		if (!(paramsMap.find("smoothness") == paramsMap.end()))
			smoothness = stof(paramsMap.find("smoothness")->second);
		else
			smoothness = 0.5f;

	}
	catch (const exception& e) {
		cout << e.what(); // information from length_error printed
	}
}
string GameObject::toString() {
	string output = "\ngameObjectType: " + getObjectType() + ",";
	output += "\nid: " + id + ",";
	output += "\nrenderable: " + renderable + ",";
	output += "\nxPos: " + to_string(x)+ ",";
	output += "\nyPos: " + to_string(y) + ",";
	output += "\norientation: " + to_string(orientation) + ",";
	output += "\nwidth: " + to_string(width) + ",";
	output += "\nlength: " + to_string(length) + ",";
	//output += "\nphysicsEnabled: " + to_string(physicsEnabled) + ",";
	//output += "\nwindScale: " + to_string(windScale) + ",";
	output += "\nimageFrames: " + to_string(imageFrames) + ",";
	output += "\nrenderType: " + to_string((int)renderType) + ",";
	output += "\nmodel: " + model + ",";
	output += "\nnormalMap: " + normalMap + ",";
	output += "\nsmoothness: " + to_string(smoothness) + ",";
	return output;
}

string GameObject::getObjectType() {
	return "GameObject";
}

void GameObject::earlyUpdate() {

}

void GameObject::midUpdate() {

}

void GameObject::lateUpdate() {

}

void GameObject::onCollide(GameObject* otherObj) {
	//OutputDebugString(id.c_str());
	//OutputDebugString( " COLLIDED WITH " );
	//OutputDebugString(otherObj->id.c_str());
	//OutputDebugString("\n");

	//objData->toDestroyVector.push_back(this);
}

//sets the object's (x,y) position to the coordinates specified by the vector2 parameter
void GameObject::setPostion(Vector2 posVector) {
	x = posVector.x;
	y = posVector.y;
}

RenderableType GameObject::getRenderableTypeFromName(std::string name)
{
	//TODO find a better way to do this

	if (name == "object3d" || name == "OBJECT3D" || name == "Object3D")
	{
		return RenderableType::OBJECT3D;
	}
	else if (name == "forward3d" || name == "FORWARD3D" || name == "Forward3D" || name == "forward" || name == "FORWARD")
	{
		return RenderableType::FORWARD3D;
	}
	else if (name == "billboard" || name == "BILLBOARD" || name == "Billboard")
	{
		return RenderableType::BILLBOARD;
	}
	else if (name == "overlay" || name == "OVERLAY" || name == "Overlay")
	{
		return RenderableType::OVERLAY;
	}

	return RenderableType::OBJECT3D;
}

void GameObject::setPosition(float newX, float newY, float newZ, int rotation) {
	if(rotation < 99999)
		offsetPosition(newX - x, newY - y, newZ - z, rotation);
	else
		offsetPosition(newX - x, newY - y, newZ - z);
}

void GameObject::offsetPosition(float offsetX, float offsetY, float offsetZ, int rotation) {
	if (!childObjects.empty()) {
		for (GameObject* g : childObjects) {
			g->offsetPosition(offsetX, offsetY, offsetZ, rotation);
		}
	}
	x += offsetX;
	y += offsetY;
	z += offsetZ;
	orientation += rotation;

	std::ostringstream oss;
	Msg* mm = new Msg(UPDATE_OBJECT_POSITION, "");

	//UPDATE_OBJECT_POSITION, //id,renderable,x,y,z,orientation,width,length,type
	oss << id << ","
		<< renderable << ","
		<< x << ","
		<< y << ","
		<< z << ","
		<< orientation << ","
		<< width << ","
		<< length << ","
		<< getObjectType();

	mm->data = oss.str();
	objData->toPostVector.push_back(mm);
}

void GameObject::setParent(GameObject* newParent) {
	parentObject = newParent;
	parentObject->childObjects.push_back(this);
	//parentObject->offsetPosition(300, 300, 0);
}

void GameObject::addChild(GameObject* newChild) {
	childObjects.push_back(newChild);
}

//remove a child reference. Does NOT delete the child, just the pointer. 
//return true if child pointer successfully removed
bool GameObject::removeChild(GameObject* child2Remove) {
	for (GameObject* g : childObjects) {
		if (g == child2Remove) {
			childObjects.erase(remove(childObjects.begin(), childObjects.end(), g), childObjects.end());
			return true;
		}
	}
	return false;
}
//destroy the objects with its children
void GameObject::destroyWithChildren() {
	for (GameObject* g : childObjects) {
		g->destroyWithChildren();
	}
	parentObject->removeChild(this);
	objData->toDestroyVector.push_back(this);
}