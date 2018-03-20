#pragma once
#include "GameObject.h"


class FullscreenObj :
	public GameObject {
public:
	FullscreenObj();
	FullscreenObj(int idNum, std::string renderableName, float xpos, float ypos, int orientation, GameSystemUtil* _gameSystemUtil);
	//DummyGameObj(std::vector<std::string> params);
	FullscreenObj(map<string, string> params, GameSystemUtil* _gameSystemUtil);
	~FullscreenObj();
	std::string getObjectType();
	std::string toString();
	void earlyUpdate();
	void midUpdate();
	void lateUpdate();

//	int sail;//the sail position 0-2
//	int rudder;// 0-4 rudder position

	int counter = 0;
};

