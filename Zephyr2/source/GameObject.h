#pragma once
#include "Msg.h"
#include <map>
#include <iostream>
#include "ObjectData.h"
#include <Windows.h>
using namespace std;

class GameObject {
public:
	std::string id;
	int x;
	int y;
	int z;
	int width;
	int length;
	int orientation; // in degrees, 0 pointing up on the screen, clockwise rotation = positive
	int physicsEnabled; // 0 or 1.
	int windScale; //how effected the object is by wind 0-1
	int imageFrames;
	
	// in degrees, same as orientation; difference is that orientation is used for rendering, direction is the direction of movement
	// this CAN BE DIFFERENT from orientation; eg: cannon ball orientated one direction, but affecteed by the wind will curve.
	int direction; 
	ObjectData* objData;
	std::string renderable; // name of a file/sprite? probably needs to be changed

	GameObject();
	~GameObject();
	GameObject::GameObject(std::map <std::string, std::string> paramsMap, ObjectData* objData);
	virtual std::string getObjectType();
	virtual std::string toString();
	void earlyUpdate();
	virtual void midUpdate();
	void lateUpdate();
	virtual void onCollide(GameObject* otherObj);
	GameObject* parentObject;
protected:

private:

};