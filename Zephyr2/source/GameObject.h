#pragma once
#include <windows.h>
#include <map>
#include <iostream>
#include "Vector2.h"
#include "renderer\RenderableTypes.h"
#include "GameSystemUtil.h"
using namespace std;

class GameObject {
public:
	std::string id;
	float x;
	float y;
	float z;
	float width;
	float length;
	float height = 1.0f;
	int originalWidth; // necessary for the scale of the healthbar. Also for scaling maybe?
	int originalLength;
	//int orientation; // in degrees, 0 pointing up on the screen, clockwise rotation = positive
	float xRotation = 0, yRotation = 0, zRotation = 0;

	//int physicsEnabled; // 0 or 1.
	//int windScale; //how effected the object is by wind 0-1
	int imageFrames;
	int animationDelay = 1;
	bool animateOnce = false;

	// in degrees, same as orientation; difference is that orientation is used for rendering, direction is the direction of movement
	// this CAN BE DIFFERENT from orientation; eg: cannon ball orientated one direction, but affecteed by the wind will curve.
	int direction; 

	//The object's default parent. Only used when loading/saving from file. 
	//use parentObject* instead of this, unless you know what you're doing
	std::string parentId = "";

	std::string renderable;
	RenderableType renderType;
	std::string model;
	std::string normalMap;
	float smoothness;

	GameSystemUtil* gameSystemUtil;//pointer to the active gamesystem. only has access to utility functions not the rest

	GameObject();
	~GameObject();
	GameObject(std::map <std::string, std::string> paramsMap, GameSystemUtil* _gameSystemUtil);
	virtual std::string getObjectType();
	virtual std::string toString();
	void earlyUpdate();
	virtual void midUpdate();
	void lateUpdate();
	virtual void onCollide(GameObject* otherObj);

	//sets the object's (x,y) position to the coordinates specified by the vector2 parameter
	void setPostion(Vector2 posVector); 
	
	void setPosition(float _x, float _y, float _z, float rotation = 999999);

	void offsetPosition(float offsetX, float offsetY, float offsetZ, float rotation = 0);

	void setParent(GameObject* newParent);

	void addChild(GameObject* newChild);

	bool removeChild(GameObject* child2Remove);

	void destroyWithChildren();

	GameObject* parentObject = nullptr;
	std::vector<GameObject*> childObjects;

	static RenderableType getRenderableTypeFromName(std::string name);

protected:
	

private:

};