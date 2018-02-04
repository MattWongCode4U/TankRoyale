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
		z = stoi(paramsMap.find("zPos")->second);
		orientation = stoi(paramsMap.find("orientation")->second);
		width = stoi(paramsMap.find("width")->second);
		length = stoi(paramsMap.find("length")->second);
		//physicsEnabled = stoi(paramsMap.find("physicsEnabled")->second);
		//windScale = stoi(paramsMap.find("windScale")->second);
		imageFrames = stoi(paramsMap.find("imageFrames")->second);
		if (!(paramsMap.find("renderType") == paramsMap.end()))
			renderType = getRenderableTypeFromName(paramsMap.find("renderType")->second);
		else
			renderType = RenderableType::OBJECT3D;
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
