#pragma once
#include "Scene.h"

class Scene_Gameplay : public Scene {
public:
	Scene_Gameplay(MessageBus* _mbus, GameSystem* _gs);
	~Scene_Gameplay();

	//Runs once per gameloop frame
	void sceneUpdate();

	//runs whenever a message is received by GameSystem
	void sceneHandleMessage(Msg * msg);

	//code that runs once when  scene is loaded goes here
	void startScene();
};

