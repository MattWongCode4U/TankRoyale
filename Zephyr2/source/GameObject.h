#pragma once
#include <windows.h>
#include <map>
#include <iostream>
#include "ObjectData.h"
#include "Vector2.h"
#include "renderer\RenderableTypes.h"
using namespace std;

class GameObject {
public:
	std::string id;
	float x;
	float y;
	int z;
	int width;
	int length;
	int originalWidth; // necessary for the scale of the healthbar. Also for scaling maybe?
	int originalLength;
	int orientation; // in degrees, 0 pointing up on the screen, clockwise rotation = positive
	//int physicsEnabled; // 0 or 1.
	//int windScale; //how effected the object is by wind 0-1
	int imageFrames;
	
	// in degrees, same as orientation; difference is that orientation is used for rendering, direction is the direction of movement
	// this CAN BE DIFFERENT from orientation; eg: cannon ball orientated one direction, but affecteed by the wind will curve.
	int direction; 
	ObjectData* objData;

	std::string renderable;
	RenderableType renderType;
	std::string model;
	std::string normalMap;
	float smoothness;

	GameObject();
	~GameObject();
	GameObject(std::map <std::string, std::string> paramsMap, ObjectData* objData);
	virtual std::string getObjectType();
	virtual std::string toString();
	void earlyUpdate();
	virtual void midUpdate();
	void lateUpdate();
	virtual void onCollide(GameObject* otherObj);

	//sets the object's (x,y) position to the coordinates specified by the vector2 parameter
	void setPostion(Vector2 posVector); 
	

	GameObject* parentObject;

	static RenderableType getRenderableTypeFromName(std::string name);

protected:
	

private:

};