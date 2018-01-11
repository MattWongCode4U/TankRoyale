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
		x = stoi(paramsMap.find("xPos")->second);
		y = stoi(paramsMap.find("yPos")->second);
		z = stoi(paramsMap.find("zPos")->second);
		orientation = stoi(paramsMap.find("orientation")->second);
		width = stoi(paramsMap.find("width")->second);
		length = stoi(paramsMap.find("length")->second);
		physicsEnabled = stoi(paramsMap.find("physicsEnabled")->second);
		windScale = stoi(paramsMap.find("windScale")->second);
		imageFrames = stoi(paramsMap.find("imageFrames")->second);
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
	output += "\nphysicsEnabled: " + to_string(physicsEnabled) + ",";
	output += "\nwindScale: " + to_string(windScale) + ",";
	output += "\nimageFrames: " + to_string(imageFrames) + ",";
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