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
	//The id used to identify the gameobject. loaded from the object text files, or set on object creation
	std::string id;

	//object position in world space
	float x;
	float y;
	float z;

	//the scale of the object displayed. this modifies the size of the model displayed. 
	//seperate from collider size
	float width;
	float length;
	float height = 1.0f;

	//the size of the 
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

	//the texture name
	std::string renderable;

	//how the object is rendered object3d, forward3d, billboard, or overlay
	RenderableType renderType;

	//the object model. can be blank for overlay renderTypes.
	std::string model;

	std::string normalMap;

	//used by renderer
	float smoothness;

	//pointer to the active gamesystem. only has access to utility functions not the rest
	GameSystemUtil* gameSystemUtil;

	GameObject();
	~GameObject();
	GameObject(std::map <std::string, std::string> paramsMap, GameSystemUtil* _gameSystemUtil);
	virtual std::string getObjectType();
	virtual std::string toString();
	void earlyUpdate();
	virtual void midUpdate();
	void lateUpdate();

	//are collisions enabled on the gameObject. 1= enabled 0 = disabled
	int collisionsEnabled; 

	//called by the checkColliision() in gameSystem. 
	//should get called every frame that the object is colliding with another object 
	//only used by objects with the collisions enabled flag set to true
	virtual void onCollide(GameObject* otherObj);

	//sets the object's (x,y) position to the coordinates specified by the vector2 parameter
	void setPostion(Vector2 posVector); 
	
	//sets the gameobject position. also updates its chidren and sends messages other systems
	void setPosition(float _x, float _y, float _z, float rotation = 999999);

	//adds to the gameobject position. also updates its chidren and sends messages other systems
	void offsetPosition(float offsetX, float offsetY, float offsetZ, float rotation = 0);


	//the object's parent in worldspace. can be null
	GameObject* parentObject = nullptr;

	//the objects children. will follow it around worldspace.
	std::vector<GameObject*> childObjects;

	//sets the object parent in the gameworld. it will now move to follow it's parent. 
	//not to be confused witht class hierarchy
	void setParent(GameObject* newParent);

	//pointer to new child object to follow this object around in world space. can have multiple children
	void addChild(GameObject* newChild);

	//remove the object from the list of children
	bool removeChild(GameObject* child2Remove);

	//destroy this object and all of the child objects it has
	void destroyWithChildren();


	/*move object towards the specified position in the number of frames
	params:
	float targetX, targetY, targetZ = the target position
	float turnZ = the number of degrees to turn around z direction
	int frames = number of frames to move in
	*/
	void moveTowards(float targetX, float targetY, float targetZ, float turnZ, int frames);

	//how much to offset the object by, each turn;
	float offsetX, offsetY, offsetZ, rotationOffsetZ;
	//number of frames left in current movement
	int offsetFrames = 0;

	//turn the object around the zAxis in the direction specified. optionally do it over <turnTime> number of frames
	void turn(float turnDir, int turnTime = 0);

	//sends an UPDATE_POSIION message to the bus. includes sprite info
	void postPostionMsg();

	//sends an UPDATE_SPRITE message to the bus.
	void postSpriteMsg();

	//parses the renderableTpe string in the loaded .txt
	static RenderableType getRenderableTypeFromName(std::string name);

protected:
	

private:

};