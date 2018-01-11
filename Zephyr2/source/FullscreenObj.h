#pragma once
#include "GameObject.h"


class FullscreenObj :
	public GameObject {
public:
	FullscreenObj(int idNum, std::string renderableName, int xpos, int ypos, int orientation);
	//DummyGameObj(std::vector<std::string> params);
	FullscreenObj(map<string, string> params, ObjectData* objData);
	~FullscreenObj();
	virtual std::string getObjectType();
	virtual std::string toString();
	void earlyUpdate();
	void midUpdate();
	void lateUpdate();

	int sail;//the sail position 0-2
	int rudder;// 0-4 rudder position

	int counter = 0;
};

