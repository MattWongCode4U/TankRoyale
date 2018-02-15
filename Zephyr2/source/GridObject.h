#pragma once
#include "GameObject.h"
#include "GridVector3.h"

class GridObject : public GameObject {
public:
	GridObject(map<string, string> params, ObjectData* objData);

	GridObject(ObjectData* _objData, std::string id, std::string renderable, float xPos, float yPos, int zPos, int orientation, int width, int length, int imageFrames, int gridX, int gridY, std::string _parent);
	
	~GridObject();

	//returns the object type string e.g. "GridObject"
	std::string getObjectType();

	//returns the string representation of the objects data. Compatible with save file syntax
	std::string toString();
	
	//update methods. called by gamesystem in the gameloop
	void GridObject::earlyUpdate();
	void GridObject::midUpdate();
	void GridObject::lateUpdate();

	//updates the world coordinates to match the grid coordinates
	void updateWorldCoords();

	//set objects coordinates in grid space. Automatically updates the world space as well
	void setGridCoords(int _gridX, int _gridY);
	

	//object's position in in grid coordinates
	int gridX;
	int gridY;

	GridVector3 gridPos;

	const int hexSize = 50;
};