#include "GameObject.h"
using namespace std;
GameObject::GameObject() {
}

GameObject::~GameObject() {
}
GameObject::GameObject(map <string, string> paramsMap, GameSystemUtil* _gameSystemUtil) {
	gameSystemUtil = _gameSystemUtil;
	try {
		id = paramsMap.find("id")->second;
		renderable = paramsMap.find("renderable")->second;
		
		x = stof(paramsMap.find("xPos")->second);
		y = stof(paramsMap.find("yPos")->second);
		z = stof(paramsMap.find("zPos")->second);
		if (!(paramsMap.find("xRotation") == paramsMap.end()))
			xRotation = stof(paramsMap.find("xRotation")->second);
		else
			xRotation = 0;

		if (!(paramsMap.find("yRotation") == paramsMap.end()))
			yRotation = stof(paramsMap.find("yRotation")->second);
		else
			yRotation = 0;

		if (!(paramsMap.find("zRotation") == paramsMap.end()))
			zRotation = stof(paramsMap.find("zRotation")->second);
		else
			zRotation = 0;
		width = stof(paramsMap.find("width")->second);
		length = stof(paramsMap.find("length")->second);

		if (!(paramsMap.find("height") == paramsMap.end())) 
			height = stof(paramsMap.find("height")->second);
		else
			height = 1;
		
		originalWidth = width;
		originalLength = length;
		if (!(paramsMap.find("parentId") == paramsMap.end()))
			parentId = paramsMap.find("parentId")->second;

		imageFrames = stoi(paramsMap.find("imageFrames")->second);

		if (!(paramsMap.find("animationDelay") == paramsMap.end()))
			animationDelay = stoi(paramsMap.find("animationDelay")->second);

		if (!(paramsMap.find("animateOnce") == paramsMap.end()))
			animateOnce = stoi(paramsMap.find("animateOnce")->second);

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
	output += "\norientation: " + to_string(zRotation) + ",";
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
	if (offsetFrames > 0) {
		offsetPosition(offsetX, offsetY, offsetZ, rotationOffsetZ);
		offsetFrames--;
	}
		
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

void GameObject::setPosition(float newX, float newY, float newZ, float rotation) {
	if(rotation < 99999)
		offsetPosition(newX - x, newY - y, newZ - z, rotation);
	else
		offsetPosition(newX - x, newY - y, newZ - z);
}

void GameObject::offsetPosition(float offsetX, float offsetY, float offsetZ, float rotation) {
	if (!childObjects.empty()) {
		for (GameObject* g : childObjects) {
			g->offsetPosition(offsetX, offsetY, offsetZ, rotation);
		}
	}
	x += offsetX;
	y += offsetY;
	z += offsetZ;
	zRotation += rotation;

	postPostionMsg();
}

void GameObject::setParent(GameObject* newParent) {
	parentObject = newParent;
	parentObject->childObjects.push_back(this);

	offsetPosition(parentObject->x, parentObject->y, parentObject->z, parentObject->zRotation);
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
	gameSystemUtil->deleteGameObject(this);
}

/*move object towards the specified position in the number of frames
params:
float targetX, targetY, targetZ = the target position
float turnZ = the number of degrees to turn around z direction
int frames = number of frames to move in
*/
void GameObject::moveTowards(float targetX, float targetY, float targetZ, float turnZ, int frames) {
	//if (offsetFrames <= 0) return; //if already moving towards, ignore
	offsetFrames = frames;
	offsetX = (targetX-x) / (float)frames;
	offsetY = (targetY-y) / (float)frames;
	offsetZ = (targetZ-z) / (float)frames;
	//float newRotation = std::fmod((turnZ - zRotation),360);
	rotationOffsetZ = (turnZ - zRotation) / (float)frames;
}


void GameObject::postPostionMsg() {
	ostringstream oss;
	Msg* mm = new Msg(UPDATE_OBJECT_POSITION, "");

	//UPDATE_OBJECT_POSITION, //id,renderable,x,y,z,orientation,width,length,type
	oss << id << ","
		<< renderable << ","
		<< x << ","
		<< y << ","
		<< z << ","
		<< xRotation << ","
		<< yRotation << ","
		<< zRotation << ","
		<< width << ","
		<< length << ","
		<< height << ","
		<< getObjectType();

	mm->data = oss.str();

	gameSystemUtil->postMessageToBus(mm);
}

void GameObject::postSpriteMsg() {
	ostringstream oss;
	Msg* mm = new Msg(UPDATE_OBJ_SPRITE, "");

	//UPDATE_OBJ_SPRITE, id,Frames,renderable
	oss << id << ","
		<< "1,"
		<< renderable;
	
	mm->data = oss.str();

	gameSystemUtil->postMessageToBus(mm);
}

void GameObject::turn(float turnDir, int turnTime) {
	//offsetPosition(0, 0, 0, turnDir);
	float newOrientation = turnDir + zRotation;
	newOrientation = std::fmod(newOrientation,360);
	moveTowards(x, y, z, newOrientation, turnTime);
}